#pragma once

#include <filesystem>
#include <string>

namespace tuim
{
  class Song
  {
  public:
    Song(const std::filesystem::path &i_path, const std::string &i_title, const std::string &i_artist);

  public:
    std::filesystem::path path = "";
    std::string title = "";
    std::string artist = "";
  };
}
