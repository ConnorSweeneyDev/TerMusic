#include "database.hpp"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  Database::Table::Table(const std::string &i_name, const std::vector<std::pair<std::string, std::string>> &i_columns)
    : name(i_name), columns(i_columns),
      definition(
        [&]()
        {
          std::string i_definition = name + "(";
          for (size_t i = 0; i < columns.size(); i++)
            i_definition += columns[i].first + " " + columns[i].second + ((i == columns.size() - 1) ? ")" : ", ");
          return i_definition;
        }())
  {
  }

  Database::Database(const std::string &name)
  {
    std::filesystem::path path(name);
    if (!path.has_filename() && path.extension() != ".db")
    {
      std::cerr << std::format("Invalid database name: {}", name) << std::endl;
      exit(EXIT_FAILURE);
    }
    if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path()))
      std::filesystem::create_directories(path.parent_path());

    if (sqlite3_open(name.c_str(), &database) != SQLITE_OK)
    {
      std::cerr << std::format("Failed to open database: {}", sqlite3_errmsg(database)) << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  Database::~Database()
  {
    sqlite3_close(database);
    database = nullptr;
  }

  void Database::bind_parameters(sqlite3_stmt *stmt, const std::vector<Database_variant> &params)
  {
    for (size_t index = 0; index < params.size(); ++index)
    {
      std::visit(
        [&](auto &value)
        {
          int column = static_cast<int>(index + 1);
          int result = SQLITE_ERROR;

          if constexpr (std::is_same_v<std::nullptr_t, std::decay_t<decltype(value)>>)
            result = sqlite3_bind_null(stmt, column);
          else if constexpr (std::is_same_v<int, std::decay_t<decltype(value)>>)
            result = sqlite3_bind_int(stmt, column, value);
          else if constexpr (std::is_same_v<double, std::decay_t<decltype(value)>>)
            result = sqlite3_bind_double(stmt, column, value);
          else if constexpr (std::is_same_v<std::string, std::decay_t<decltype(value)>>)
            result = sqlite3_bind_text(stmt, column, value.c_str(), -1, SQLITE_TRANSIENT);
          else
          {
            std::cerr << std::format("Unsupported param type: {}", typeid(value).name()) << std::endl;
            exit(EXIT_FAILURE);
          }

          if (result != SQLITE_OK)
          {
            std::cerr << std::format("Failed to bind parameter {}: {}", index + 1, sqlite3_errmsg(database))
                      << std::endl;
            exit(EXIT_FAILURE);
          }
        },
        params[index]);
    }
  }
}
