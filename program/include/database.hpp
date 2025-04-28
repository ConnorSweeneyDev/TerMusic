#pragma once

#include <string>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  class Database
  {
  public:
    Database(const std::string &name);
    ~Database();

    void execute(const std::string &sql, const std::vector<std::string> &params = {});
    template <typename Type>
    std::vector<Type> query(const std::string &sql, const std::vector<std::string> &params = {});

  private:
    sqlite3 *db = nullptr;
  };
}

#include "database.inl"
