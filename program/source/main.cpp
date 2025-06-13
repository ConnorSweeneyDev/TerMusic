#include "main.hpp"

#include <cstdlib>
#include <filesystem>
#include <future>
#include <iostream>
#include <vector>

#include "ftxui/component/loop.hpp"

#include "database.hpp"
#include "ffmpeg.hpp"
#include "interface.hpp"
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
    futures.emplace_back(std::async(std::launch::async,
                                    [song_path = entry.path().string()]()
                                    {
                                      std::vector<tuim::Song> duplicate_songs = tuim::database.query<tuim::Song>(
                                        "SELECT * FROM songs WHERE path = ?;", song_path);
                                      if (!duplicate_songs.empty()) return;
                                      tuim::FFmpeg::Tags tags = tuim::ffmpeg.get_tags(song_path);
                                      tuim::database.execute("INSERT INTO songs VALUES (?, ?, ?, ?, ?, ?);", song_path,
                                                             tags.artist, tags.title, 0.0, 0.0, 0);
                                    }));
  }
  for (std::future<void> &future : futures) future.get();
  tuim::database.execute("COMMIT;");

  tuim::interface.song_menu.populate(
    tuim::database.query<tuim::Song>("SELECT * FROM songs ORDER BY LOWER(artist) ASC, LOWER(title) ASC"));
  for (ftxui::Loop loop = tuim::interface.create_loop(); !loop.HasQuitted();)
  {
    std::vector<tuim::Song> target_songs = {};
    target_songs = tuim::database.query<tuim::Song>(
      "SELECT * FROM songs WHERE plays < (SELECT MAX(plays) FROM songs) ORDER BY RANDOM() LIMIT 1");
    if (target_songs.empty())
      target_songs = tuim::database.query<tuim::Song>("SELECT * FROM songs ORDER BY RANDOM() LIMIT 1");
    if (target_songs.empty())
    {
      std::cerr << "No songs found!" << std::endl;
      exit(EXIT_FAILURE);
    }
    tuim::Song &target_song = target_songs.front();

    tuim::player.play(target_song);
    while (tuim::player.music_active())
    {
      if (loop.HasQuitted()) break;
      tuim::interface.screen.RequestAnimationFrame();
      loop.RunOnce();
    }
    tuim::player.unload();
  }

  return 0;
}
