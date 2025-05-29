#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace tuim
{
  class Song
  {
  public:
    Song(const std::filesystem::path &i_path, const std::string &i_artist, const std::string &i_title);

  public:
    std::filesystem::path path = {};
    std::string artist = "";
    std::string title = "";

    inline static const int column_count = 3;
    inline static const std::string table_name = "songs";
    inline static const std::vector<std::string> table_columns = {"path", "artist", "title"};
    inline static const std::string table_definition = table_name + "(" + table_columns[0] + " TEXT PRIMARY KEY, " +
                                                       table_columns[1] + " TEXT NOT NULL, " + table_columns[2] +
                                                       " TEXT NOT NULL)";
    inline static const std::string table_reference =
      table_name + "(" + table_columns[0] + ", " + table_columns[1] + ", " + table_columns[2] + ")";
  };
}
