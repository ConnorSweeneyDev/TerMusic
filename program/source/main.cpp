#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "SDL2/SDL_timer.h"
#include "taglib/fileref.h"
#include "taglib/tstring.h"

#include "database.hpp"
#include "player.hpp"
#include "song.hpp"

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++) std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    exit(1);
  }

  tuim::Database database("user/TerMusic.db");
  database.execute("CREATE TABLE IF NOT EXISTS songs (path TEXT PRIMARY KEY, title TEXT, artist TEXT);");

  std::filesystem::path path = "C:/Users/conno/Music/Songs";
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
  {
    std::cerr << "Invalid path: " << path << std::endl;
    exit(1);
  }

  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_regular_file() || entry.path().extension() != ".mp3") continue;

    TagLib::FileRef file_reference(entry.path().string().c_str());
    if (file_reference.isNull())
    {
      std::cout << "FileRef Error: " << entry.path().string() << " could not be loaded." << std::endl;
      exit(1);
    }
    TagLib::String title_tag = file_reference.tag()->title();
    std::string title = title_tag.to8Bit(true);
    TagLib::String artist_tag = file_reference.tag()->artist();
    std::string artist = artist_tag.to8Bit(true);

    database.execute("INSERT OR IGNORE INTO songs (path, title, artist) VALUES (?, ?, ?);",
                     {entry.path().string(), title, artist});
  }

  std::vector<tuim::Song> songs = database.query<tuim::Song>("SELECT * FROM songs;");
  std::cout << songs.size() << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dis(0, songs.size() - 1);
  size_t index = dis(gen);
  std::cout << songs[index].path << std::endl;
  std::cout << songs[index].title << std::endl;
  std::cout << songs[index].artist << std::endl;

  tuim::Player player;
  player.set_volume(20);
  player.load(songs[index].path);
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
