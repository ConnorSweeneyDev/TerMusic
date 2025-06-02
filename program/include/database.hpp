#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  using Database_variant = std::variant<std::nullptr_t, int, double, std::string>;

  class Database
  {
    friend class Song;

  private:
    struct Table
    {
      Table(const std::string &name, const std::vector<std::pair<std::string, std::string>> &columns);

      const std::string name;
      const std::vector<std::pair<std::string, std::string>> columns;
      const std::string definition;
      const std::string reference;
    };

  public:
    Database(const std::string &name);
    ~Database();

    template <typename... Args> void execute(const std::string &sql, Args &&...args);
    template <typename Type, typename... Args> std::vector<Type> query(const std::string &sql, Args &&...args);

  private:
    void bind_parameters(sqlite3_stmt *stmt, const std::vector<Database_variant> &params);
    template <typename Type> static Type get(const Database_variant &value);

  private:
    sqlite3 *database = nullptr;
    std::mutex mutex;
  };

  inline Database database("user/database/main.db");
}

#include "database.inl"
