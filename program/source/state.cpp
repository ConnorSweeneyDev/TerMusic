#include "state.hpp"

#include <string>
#include <vector>

#include "database.hpp"

namespace tuim
{
  State::State() {}

  State::State(const std::vector<Database_variant> &columns)
    : id(Database::get<int>(columns[0])), song_path(Database::get<std::string>(columns[1])),
      song_percentage(Database::get<int>(columns[2])), volume_percentage(Database::get<int>(columns[3]))
  {
  }
}
