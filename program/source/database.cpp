#include "database.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  Database::Database(const std::string &name)
  {
    std::filesystem::path path(name);
    if (!path.has_filename() && path.extension() != ".db")
    {
      std::cerr << "Invalid database name: " << name << std::endl;
      exit(EXIT_FAILURE);
    }
    if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path()))
      std::filesystem::create_directories(path.parent_path());

    if (sqlite3_open(name.c_str(), &database) != SQLITE_OK)
    {
      std::cerr << "Failed to open database: " << std::string(sqlite3_errmsg(database)) << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  Database::~Database()
  {
    sqlite3_close(database);
    database = nullptr;
  }

  void Database::execute(const std::string &sql, const std::vector<std::variant<std::string, int, double>> &params)
  {
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database) << std::endl;
      exit(EXIT_FAILURE);
    }
    bind_parameters(stmt, params);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      std::cerr << "Failed to execute statement: " << sqlite3_errmsg(database) << std::endl;
      exit(EXIT_FAILURE);
    }
    sqlite3_finalize(stmt);
  }

  void Database::bind_parameters(sqlite3_stmt *stmt, const std::vector<std::variant<std::string, int, double>> &params)
  {
    for (size_t index = 0; index < params.size(); ++index)
    {
      std::visit(
        [&](auto &value)
        {
          int column = static_cast<int>(index + 1);
          int result = SQLITE_ERROR;

          if constexpr (std::is_same<std::string, std::decay_t<decltype(value)>>::value)
            result = sqlite3_bind_text(stmt, column, value.c_str(), -1, SQLITE_TRANSIENT);
          else if constexpr (std::is_same<int, std::decay_t<decltype(value)>>::value)
            result = sqlite3_bind_int(stmt, column, value);
          else if constexpr (std::is_same<double, std::decay_t<decltype(value)>>::value)
            result = sqlite3_bind_double(stmt, column, value);
          else
            std::cerr << "Unsupported param type: " << typeid(value).name() << std::endl;

          if (result != SQLITE_OK)
          {
            std::cerr << "Failed to bind parameter " << index + 1 << ": " << sqlite3_errmsg(database) << std::endl;
            exit(EXIT_FAILURE);
          }
        },
        params[index]);
    }
  }
}
