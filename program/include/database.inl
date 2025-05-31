#pragma once

#include "database.hpp"

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  template <typename Type> std::vector<Type>
  Database::query(const std::string &sql,
                  const std::vector<std::variant<std::nullptr_t, std::string, int, long long, double>> &params)
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
        if (column_indices.find(column) == column_indices.end())
        {
          std::cerr << "Missing required column \"" << column << "\" in query: " << typeid(Type).name() << std::endl;
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
        Type::handle_query(results, stmt, column_indices);
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
