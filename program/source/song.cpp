#include "song.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include "database.hpp"

namespace tuim
{
  Song::Song(const std::vector<Database_variant> &columns)
  {
    path = std::get<std::string>(columns[0]);
    artist = std::get<std::string>(columns[1]);
    title = std::get<std::string>(columns[2]);
    mean_volume = std::get<double>(columns[3]);
  }
}
