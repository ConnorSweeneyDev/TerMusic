#pragma once

#include <filesystem>
#include <vector>

#include "database.hpp"

namespace tuim
{
  class State
  {
  public:
    State();
    State(const std::vector<Database_variant> &columns);

  public:
    const int id = 0;
    std::filesystem::path song_path = {};
    int song_percentage = 0;
    int volume_percentage = 10;

    inline static const Database::Table table = {"state",
                                                 {
                                                   {"id", "INTEGER PRIMARY KEY"},
                                                   {"song_path", "TEXT NOT NULL"},
                                                   {"song_percentage", "INTEGER NOT NULL"},
                                                   {"volume_percentage", "INTEGER NOT NULL"},
                                                 }};
  };
}
