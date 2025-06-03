#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <vector>

#include "SDL2/SDL_timer.h"

#include "database.hpp"
#include "ffmpeg.hpp"
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

  std::vector<std::future<void>> futures = {};
  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::Song::table.definition + ";");
  tuim::database.execute("BEGIN TRANSACTION;");
  for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_regular_file() || entry.path().extension() != ".mp3") continue;
    futures.emplace_back(std::async(
      std::launch::async,
      [song_path = entry.path().string()]()
      {
        std::vector<tuim::Song> duplicate_songs =
          tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table.name + " WHERE path = ?;", song_path);
        if (!duplicate_songs.empty()) return;
        tuim::FFmpeg::Tags tags = tuim::ffmpeg.get_tags(song_path);
        tuim::database.execute("INSERT INTO " + tuim::Song::table.reference + " VALUES (?, ?, ?, ?);", song_path,
                               tags.artist, tags.title, 0.0);
      }));
  }
  for (std::future<void> &future : futures) future.get();
  tuim::database.execute("COMMIT;");

  std::vector<tuim::Song> target_songs =
    tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table.name + " ORDER BY RANDOM()");
  if (target_songs.empty())
  {
    std::cerr << "No songs found!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << target_songs.size() << std::endl;

  tuim::Player player;
  player.set_volume(20);
  for (tuim::Song &target_song : target_songs)
  {
    if (target_song.mean_volume == 0.0)
    {
      tuim::ffmpeg.update_mean_volume(target_song.path.string());
      target_song.mean_volume = tuim::ffmpeg.last_mean_volume;
      tuim::database.execute("UPDATE " + tuim::Song::table.name + " SET mean_volume = ? WHERE path = ?;",
                             target_song.mean_volume, target_song.path.string());
    }

    std::cout << target_song.path.string() << std::endl;
    std::cout << target_song.artist << std::endl;
    std::cout << target_song.title << std::endl;
    std::cout << target_song.mean_volume << std::endl;

    player.load(target_song.path);
    player.play();
    while (player.music_active()) SDL_Delay(1000);
    player.unload();
  }

  std::cout << "Done!" << std::endl;
  return 0;
}
