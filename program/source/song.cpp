#include "song.hpp"

#include <string>
#include <vector>

#include "database.hpp"

namespace tuim
{
  Song::Song() {}

  Song::Song(const std::vector<Database_variant> &columns)
    : path(Database::get<std::string>(columns[0])), artist(Database::get<std::string>(columns[1])),
      title(Database::get<std::string>(columns[2])), duration(Database::get<double>(columns[3])),
      mean_volume(Database::get<double>(columns[4])), plays(Database::get<int>(columns[5]))
  {
  }

  bool Song::operator==(const Song &other) const { return path == other.path; }
}
