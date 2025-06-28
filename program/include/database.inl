#pragma once

#include "database.hpp"

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  template <typename... Args> void Database::execute(const std::string &sql, Args &&...args)
  {
    std::lock_guard<std::mutex> lock(mutex);

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << std::format("Failed to prepare statement: {}", sqlite3_errmsg(database)) << std::endl;
      exit(EXIT_FAILURE);
    }
    bind_parameters(stmt, std::vector<Database_variant>{Database_variant(std::forward<Args>(args))...});

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      std::cerr << std::format("Failed to execute statement: {}", sqlite3_errmsg(database)) << std::endl;
      exit(EXIT_FAILURE);
    }
    sqlite3_finalize(stmt);
  }

  template <typename Type, typename... Args> std::vector<Type> Database::query(const std::string &sql, Args &&...args)
  {
    std::lock_guard<std::mutex> lock(mutex);

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << std::format("Failed to prepare statement: {}", sqlite3_errmsg(database)) << std::endl;
      exit(EXIT_FAILURE);
    }
    bind_parameters(stmt, std::vector<Database_variant>{Database_variant(std::forward<Args>(args))...});

    std::unordered_map<std::string, int> column_indices;
    for (int index = 0; index < sqlite3_column_count(stmt); ++index)
      if (const char *name = sqlite3_column_name(stmt, index); name) column_indices[name] = index;

    try
    {
      for (const auto &column : Type::table.columns)
        if (column_indices.find(column.first) == column_indices.end())
        {
          std::cerr << std::format("Missing required column \"{}\" in query: {}", column.first, typeid(Type).name())
                    << std::endl;
          exit(EXIT_FAILURE);
        }
      if (Type::table.columns.size() != sqlite3_column_count(stmt))
      {
        std::cerr << std::format("Invalid column count {} (should be {}) in query: {}", sqlite3_column_count(stmt),
                                 Type::table.columns.size(), typeid(Type).name())
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    catch (const std::exception &exception)
    {
      std::cerr << std::format("Failed to handle columns for type {}: {}", typeid(Type).name(), exception.what())
                << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<Type> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) try
      {
        std::vector<Database_variant> columns = {};
        for (size_t index = 0; index < column_indices.size(); ++index)
        {
          int column_type = sqlite3_column_type(stmt, column_indices[Type::table.columns[index].first]);
          switch (column_type)
          {
            case SQLITE_NULL: columns.emplace_back(nullptr); break;
            case SQLITE_INTEGER:
              columns.emplace_back(sqlite3_column_int(stmt, column_indices[Type::table.columns[index].first]));
              break;
            case SQLITE_FLOAT:
              columns.emplace_back(sqlite3_column_double(stmt, column_indices[Type::table.columns[index].first]));
              break;
            case SQLITE_TEXT:
              columns.emplace_back(reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, column_indices[Type::table.columns[index].first])));
              break;
            default:
              std::cerr << std::format("Unsupported column type {} for column {} in query: {}", column_type,
                                       Type::table.columns[index].first, typeid(Type).name())
                        << std::endl;
              exit(EXIT_FAILURE);
          }
        }
        results.emplace_back(Type(columns));
      }
      catch (const std::exception &exception)
      {
        std::cerr << std::format("Failed to handle query for type {}: {}", typeid(Type).name(), exception.what())
                  << std::endl;
        exit(EXIT_FAILURE);
      }

    sqlite3_finalize(stmt);
    return results;
  }

  template <typename Type> Type Database::get(const Database_variant &value)
  {
    if (std::holds_alternative<std::nullptr_t>(value))
    {
      if constexpr (std::is_same_v<Type, int>)
        return 0;
      else if constexpr (std::is_same_v<Type, double>)
        return 0.0;
      else if constexpr (std::is_same_v<Type, std::string>)
        return "";
      else
      {
        std::cerr << std::format("Unsupported type {} for get!", typeid(Type).name()) << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    if (std::holds_alternative<Type>(value)) return std::get<Type>(value);

    throw std::bad_variant_access();
  }
}
