#pragma once

#include "database.hpp"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

#include "song.hpp"

namespace tuim
{
  template <typename Type> std::vector<Type>
  Database::query(const std::string &sql,
                  const std::vector<std::variant<std::nullptr_t, std::string, int, long long, double>> &params)
  {
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database) << std::endl;
      exit(EXIT_FAILURE);
    }

    bind_parameters(stmt, params);
    std::unordered_map<std::string, int> column_indices;
    for (int index = 0; index < sqlite3_column_count(stmt); ++index)
    {
      const char *name = sqlite3_column_name(stmt, index);
      if (name) column_indices[name] = index;
    }
    for (const auto &field : Type::table_columns)
    {
      if (column_indices.find(field) == column_indices.end())
      {
        std::cerr << "Missing required column \"" << field << "\" in query: " << typeid(Type).name() << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    if (Type::column_count != sqlite3_column_count(stmt))
    {
      std::cerr << "Invalid column count " << sqlite3_column_count(stmt) << " (should be " << Type::column_count << ")"
                << " in query: " << typeid(Type).name() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::vector<Type> results;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      if constexpr (std::is_same_v<Type, Song>)
      {
        const char *path_text =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[Song::table_columns[0]]));
        const char *artist_text =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[Song::table_columns[1]]));
        const char *title_text =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[Song::table_columns[2]]));

        const std::filesystem::path path = path_text ? path_text : "";
        const std::string artist = artist_text ? artist_text : "";
        const std::string title = title_text ? title_text : "";
        results.emplace_back(path, artist, title);
      }
      else
      {
        std::cerr << "Unsupported query type: " << typeid(Type).name() << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    sqlite3_finalize(stmt);
    return results;
  }
}
