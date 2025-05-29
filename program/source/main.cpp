#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
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
    exit(EXIT_FAILURE);
  }

  std::filesystem::path path = "C:/Users/conno/Music/Songs";
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
  {
    std::cerr << "Invalid path: " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::Song::table_definition + ";");
  tuim::database.execute("BEGIN TRANSACTION;");
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_regular_file() || entry.path().extension() != ".mp3") continue;

    TagLib::FileRef file_reference(entry.path().string().c_str());
    if (file_reference.isNull())
    {
      std::cerr << "FileRef Error: " << entry.path().string() << " could not be loaded!" << std::endl;
      exit(EXIT_FAILURE);
    }
    TagLib::String artist_tag = file_reference.tag()->artist();
    std::string artist = artist_tag.to8Bit(true);
    TagLib::String title_tag = file_reference.tag()->title();
    std::string title = title_tag.to8Bit(true);

    tuim::database.execute("INSERT OR IGNORE INTO " + tuim::Song::table_reference + " VALUES (?, ?, ?);",
                           {entry.path().string(), artist, title});
  }
  tuim::database.execute("COMMIT;");

  std::vector<tuim::Song> target_songs =
    tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table_name + " ORDER BY RANDOM() LIMIT 1;");
  if (target_songs.empty())
  {
    std::cerr << "No songs found!" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << target_songs.size() << std::endl;
  std::cout << target_songs[0].path.string() << std::endl;
  std::cout << target_songs[0].artist << std::endl;
  std::cout << target_songs[0].title << std::endl;

  tuim::Player player;
  player.set_volume(20);
  player.load(target_songs[0].path);
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
