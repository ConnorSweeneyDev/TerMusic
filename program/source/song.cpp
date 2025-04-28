#include "song.hpp"

#include <filesystem>
#include <string>

namespace tuim
{
  Song::Song(const std::filesystem::path &i_path, const std::string &i_title, const std::string &i_artist)
    : path(i_path), title(i_title), artist(i_artist) {};
}
