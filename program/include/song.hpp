#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "database.hpp"

namespace tuim
{
  class Song
  {
  public:
    Song(const std::vector<Database_variant> &columns);

  public:
    std::filesystem::path path = {};
    std::string artist = "";
    std::string title = "";
    double mean_volume = 0.0;

    inline static const std::string table_name = "songs";
    inline static const std::vector<std::pair<std::string, std::string>> table_columns = {
      {"path", "TEXT PRIMARY KEY"},
      {"artist", "TEXT NOT NULL"},
      {"title", "TEXT NOT NULL"},
      {"mean_volume", "REAL NOT NULL"}};
    inline static const std::string table_definition =
      table_name + "(" + table_columns[0].first + " " + table_columns[0].second + ", " + table_columns[1].first + " " +
      table_columns[1].second + ", " + table_columns[2].first + " " + table_columns[2].second + ", " +
      table_columns[3].first + " " + table_columns[3].second + ")";
    inline static const std::string table_reference = table_name + "(" + table_columns[0].first + ", " +
                                                      table_columns[1].first + ", " + table_columns[2].first + ", " +
                                                      table_columns[3].first + ")";
  };
}
