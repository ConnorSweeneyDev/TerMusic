#include "database.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  Database::Database(const std::string &name)
  {
    std::filesystem::path path(name);
    if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path()))
      std::filesystem::create_directories(path.parent_path());

    if (sqlite3_open(name.c_str(), &db) != SQLITE_OK)
    {
      std::cerr << "Failed to open database: " << std::string(sqlite3_errmsg(db)) << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  Database::~Database()
  {
    if (db != nullptr) sqlite3_close(db);
    db = nullptr;
  }

  void Database::execute(const std::string &sql, const std::vector<std::string> &params)
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
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      std::cerr << "Failed to execute prepared statement: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_finalize(stmt);
      exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
  }
}
