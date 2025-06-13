#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "database.hpp"

namespace tuim
{
  class Song
  {
  public:
    Song();
    Song(const std::vector<Database_variant> &columns);

  public:
    std::filesystem::path path = {};
    std::string artist = "";
    std::string title = "";
    double duration = 0.0;
    double mean_volume = 0.0;
    int plays = 0;

    inline static const Database::Table table = {"songs",
                                                 {
                                                   {"path", "TEXT PRIMARY KEY"},
                                                   {"artist", "TEXT NOT NULL"},
                                                   {"title", "TEXT NOT NULL"},
                                                   {"duration", "REAL NOT NULL"},
                                                   {"mean_volume", "REAL NOT NULL"},
                                                   {"plays", "INTEGER NOT NULL"},
                                                 }};
  };
}
