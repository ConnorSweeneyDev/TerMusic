#include "song.hpp"

#include <filesystem>
#include <string>

namespace tuim
{
  Song::Song(const std::filesystem::path &i_path, const std::string &i_artist, const std::string &i_title)
    : path(i_path), artist(i_artist), title(i_title) {};
}
