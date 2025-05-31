#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <future>
#include <iostream>
#include <mutex>
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

  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::Song::table_definition + ";");

  std::vector<tuim::Song> songs = {};
  std::mutex songs_mutex;
  std::vector<std::future<void>> futures = {};
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_regular_file() || entry.path().extension() != ".mp3") continue;
    std::string song_path = entry.path().string();
    futures.emplace_back(std::async(std::launch::async,
                                    [song_path, &songs_mutex, &songs]()
                                    {
                                      std::vector<tuim::Song> duplicate_songs = tuim::database.query<tuim::Song>(
                                        "SELECT * FROM " + tuim::Song::table_name + " WHERE path = ?;", {song_path});
                                      if (!duplicate_songs.empty()) return;
                                      tuim::FFmpeg::Tags tags = tuim::ffmpeg.get_tags(song_path);
                                      {
                                        std::lock_guard<std::mutex> lock(songs_mutex);
                                        songs.emplace_back(song_path, tags.artist, tags.title, 0.0);
                                      }
                                    }));
  }
  for (auto &future : futures) future.get();

  tuim::database.execute("BEGIN TRANSACTION;");
  for (const auto &song : songs)
    tuim::database.execute("INSERT INTO " + tuim::Song::table_reference + " VALUES (?, ?, ?, ?);",
                           {song.path.string(), song.artist, song.title, song.mean_volume});
  tuim::database.execute("COMMIT;");

  std::vector<tuim::Song> target_songs =
    tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table_name + " ORDER BY RANDOM() LIMIT 1");
  if (target_songs.empty())
  {
    std::cerr << "No songs found!" << std::endl;
    exit(EXIT_FAILURE);
  }

  tuim::ffmpeg.get_mean_volume(target_songs[0].path.string());
  target_songs[0].mean_volume = tuim::ffmpeg.last_mean_volume;
  tuim::database.execute("UPDATE " + tuim::Song::table_name + " SET mean_volume = ? WHERE path = ?;",
                         {target_songs[0].mean_volume, target_songs[0].path.string()});

  std::cout << target_songs.size() << std::endl;
  std::cout << target_songs[0].path.string() << std::endl;
  std::cout << target_songs[0].artist << std::endl;
  std::cout << target_songs[0].title << std::endl;
  std::cout << target_songs[0].mean_volume << std::endl;

  tuim::Player player;
  player.set_volume(20);
  player.load(target_songs[0].path);
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
