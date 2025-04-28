#pragma once

#include <filesystem>
#include <string>

namespace tuim
{
  class Song
  {
  public:
    Song(const std::filesystem::path &path, const std::string &title, const std::string &artist);

  public:
    std::filesystem::path path = "";
    std::string title = "";
    std::string artist = "";
  };
}
