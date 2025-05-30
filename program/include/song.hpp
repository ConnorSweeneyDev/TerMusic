#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  class Song
  {
  public:
    Song(const std::filesystem::path &i_path, const std::string &i_artist, const std::string &i_title,
         const double &i_mean_volume);

    static void handle_query(std::vector<Song> &results, sqlite3_stmt *stmt,
                             std::unordered_map<std::string, int> &column_indices);

  public:
    std::filesystem::path path = {};
    std::string artist = "";
    std::string title = "";
    double mean_volume = 0.0;

    inline static const int column_count = 4;
    inline static const std::string table_name = "songs";
    inline static const std::vector<std::string> table_columns = {"path", "artist", "title", "mean_volume"};
    inline static const std::string table_definition = table_name + "(" + table_columns[0] + " TEXT PRIMARY KEY, " +
                                                       table_columns[1] + " TEXT NOT NULL, " + table_columns[2] +
                                                       " TEXT NOT NULL, " + table_columns[3] + " REAL NOT NULL)";
    inline static const std::string table_reference = table_name + "(" + table_columns[0] + ", " + table_columns[1] +
                                                      ", " + table_columns[2] + ", " + table_columns[3] + ")";
  };
}
