#pragma once

#include "database.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  template <typename Type>
  std::vector<Type> Database::query(const std::string &sql, const std::vector<Database_variant> &params)
  {
    std::lock_guard<std::mutex> lock(mutex);

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database) << std::endl;
      exit(EXIT_FAILURE);
    }

    bind_parameters(stmt, params);
    std::unordered_map<std::string, int> column_indices;
    for (int index = 0; index < sqlite3_column_count(stmt); ++index)
      if (const char *name = sqlite3_column_name(stmt, index); name) column_indices[name] = index;

    try
    {
      for (const auto &column : Type::table_columns)
        if (column_indices.find(column.first) == column_indices.end())
        {
          std::cerr << "Missing required column \"" << column.first << "\" in query: " << typeid(Type).name()
                    << std::endl;
          exit(EXIT_FAILURE);
        }
      if (Type::table_columns.size() != sqlite3_column_count(stmt))
      {
        std::cerr << "Invalid column count " << sqlite3_column_count(stmt) << " (should be "
                  << Type::table_columns.size() << ")"
                  << " in query: " << typeid(Type).name() << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    catch (const std::exception &exception)
    {
      std::cerr << "Failed to handle columns for type " << typeid(Type).name() << ": " << exception.what() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<Type> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) try
      {
        std::vector<Database_variant> columns = {};
        for (size_t index = 0; index < column_indices.size(); ++index)
        {
          int column_type = sqlite3_column_type(stmt, column_indices[Type::table_columns[index].first]);
          switch (column_type)
          {
            case SQLITE_NULL: columns.emplace_back(nullptr); break;
            case SQLITE_INTEGER:
              columns.emplace_back(sqlite3_column_int(stmt, column_indices[Type::table_columns[index].first]));
              break;
            case SQLITE_FLOAT:
              columns.emplace_back(sqlite3_column_double(stmt, column_indices[Type::table_columns[index].first]));
              break;
            case SQLITE_TEXT:
              columns.emplace_back(reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, column_indices[Type::table_columns[index].first])));
              break;
            default:
              std::cerr << "Unsupported column type " << column_type << " for column "
                        << Type::table_columns[index].first << " in query: " << typeid(Type).name() << std::endl;
              exit(EXIT_FAILURE);
          }
        }
        results.emplace_back(Type(columns));
      }
      catch (const std::exception &exception)
      {
        std::cerr << "Failed to handle query for type " << typeid(Type).name() << ": " << exception.what() << std::endl;
        exit(EXIT_FAILURE);
      }

    sqlite3_finalize(stmt);
    return results;
  }
}
