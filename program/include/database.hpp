#pragma once

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

    void execute(const std::string &sql, const std::vector<std::variant<std::string, int, double>> &params = {});
    template <typename Type> std::vector<Type>
    query(const std::string &sql, const std::vector<std::variant<std::string, int, double>> &params = {});

  private:
    void bind_parameters(sqlite3_stmt *stmt, const std::vector<std::variant<std::string, int, double>> &params);

  private:
    sqlite3 *database = nullptr;
  };
}

#include "database.inl"
