#include "song.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include "database.hpp"

namespace tuim
{
  Song::Song(const std::vector<Database_variant> &columns)
    : path(Database::get<std::string>(columns[0])), artist(Database::get<std::string>(columns[1])),
      title(Database::get<std::string>(columns[2])), mean_volume(Database::get<double>(columns[3]))
  {
  }
}
