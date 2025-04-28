#pragma once

#include "database.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "sqlite/sqlite3.h"

#include "song.hpp"

namespace tuim
{
  template <typename Type>
  std::vector<Type> Database::query(const std::string &sql, const std::vector<std::string> &params)
  {
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
      if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
      {
        std::cerr << "Failed to bind parameter " << i + 1 << ": " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        exit(EXIT_FAILURE);
      }
    }

    std::vector<Type> results;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      if constexpr (std::is_same<Type, Song>::value)
      {
        const char *path_text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        const char *title_text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        const char *artist_text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        if (path_text && title_text && artist_text)
        {
          const std::filesystem::path path = path_text;
          const std::string title = title_text;
          const std::string artist = artist_text;
          results.emplace_back(path, title, artist);
        }
      }
      else
      {
        std::cerr << "Unsupported query type: " << typeid(Type).name() << std::endl;
        sqlite3_finalize(stmt);
        exit(EXIT_FAILURE);
      }
    }

    sqlite3_finalize(stmt);
    return results;
  }
}
