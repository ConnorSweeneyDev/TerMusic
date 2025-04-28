#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
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

  {
    std::filesystem::path path = "C:/Users/conno/Music/Songs/The Connells - '74-'75.mp3";
    TagLib::FileRef file_reference(path.string().c_str());
    if (file_reference.isNull())
    {
      std::cerr << "FileRef Error: Failed to open file: " << path.string() << std::endl;
      exit(1);
    }
    TagLib::String title_tag = file_reference.tag()->title();
    TagLib::String artist_tag = file_reference.tag()->artist();

    database.execute("INSERT OR IGNORE INTO songs (path, title, artist) VALUES (?, ?, ?);",
                     {path.string(), title_tag.to8Bit(true), artist_tag.to8Bit(true)});
  }

  std::vector<tuim::Song> songs = database.query<tuim::Song>("SELECT * FROM songs;");
  std::cout << songs.size() << std::endl;
  std::cout << songs[0].path << std::endl;
  std::cout << songs[0].title << std::endl;
  std::cout << songs[0].artist << std::endl;

  tuim::Player player;
  player.set_volume(20);
  player.load(songs[0].path);
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
