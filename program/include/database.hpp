#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  using Database_variant = std::variant<std::nullptr_t, int, double, std::string>;

  class Database
  {
  public:
    Database(const std::string &name);
    ~Database();

    void execute(const std::string &sql, const std::vector<Database_variant> &params = {});
    template <typename Type>
    std::vector<Type> query(const std::string &sql, const std::vector<Database_variant> &params = {});

  private:
    void bind_parameters(sqlite3_stmt *stmt, const std::vector<Database_variant> &params);

  private:
    sqlite3 *database = nullptr;
    std::mutex mutex;
  };

  inline Database database("user/database/main.db");
}

#include "database.inl"
