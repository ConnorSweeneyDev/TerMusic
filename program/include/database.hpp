#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  class Database
  {
  public:
    Database(const std::string &name);
    ~Database();

    void execute(const std::string &sql,
                 const std::vector<std::variant<std::nullptr_t, std::string, int, long long, double>> &params = {});
    template <typename Type> std::vector<Type>
    query(const std::string &sql,
          const std::vector<std::variant<std::nullptr_t, std::string, int, long long, double>> &params = {});

  private:
    void bind_parameters(sqlite3_stmt *stmt,
                         const std::vector<std::variant<std::nullptr_t, std::string, int, long long, double>> &params);

  private:
    sqlite3 *database = nullptr;
  };

  inline Database database("user/database/main.db");
}

#include "database.inl"
