#include "main.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ftxui/component/loop.hpp"

#include "database.hpp"
#include "ffmpeg.hpp"
#include "interface.hpp"
#include "player.hpp"
#include "song.hpp"
#include "state.hpp"

int main()
{
  initialize_playlist();
  load_state();
  return run_loop();
}

void initialize_playlist()
{
  std::ifstream playlist_file("user/playlist.txt");
  if (!playlist_file.is_open())
  {
    std::cerr << "Failed to open playlist file!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string line;
  std::getline(playlist_file, line);
  std::filesystem::path path(line);
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
  {
    std::cerr << "Invalid directory: " << path << std::endl;
    exit(EXIT_FAILURE);
  }
  playlist_file.close();

  std::vector<std::filesystem::path> files = {};
  for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
    if (entry.is_regular_file() && entry.path().extension() == ".mp3") files.emplace_back(entry.path());

  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::Song::table.definition + ";");
  int max_plays = 0;
  std::vector<tuim::Song> existing_songs = tuim::database.query<tuim::Song>("SELECT * FROM songs ORDER BY plays DESC;");
  if (!existing_songs.empty())
  {
    max_plays = std::max(0, existing_songs.front().plays - 1);

    std::vector<bool> to_remove(existing_songs.size());
    std::for_each(std::execution::par, existing_songs.begin(), existing_songs.end(),
                  [&](const tuim::Song &song)
                  {
                    size_t index = static_cast<size_t>(&song - existing_songs.data());
                    for (const std::filesystem::path &file : files)
                      if (file == song.path)
                      {
                        to_remove[index] = true;
                        break;
                      }
                  });
    std::erase_if(existing_songs,
                  [&](const tuim::Song &song)
                  {
                    size_t index = static_cast<size_t>(&song - existing_songs.data());
                    return to_remove[index];
                  });
  }

  tuim::database.execute("BEGIN TRANSACTION;");
  for (const tuim::Song &song : existing_songs)
    tuim::database.execute("DELETE FROM songs WHERE path = ?;", song.path.string());
  std::for_each(std::execution::par, files.begin(), files.end(),
                [&](const std::filesystem::path &file)
                {
                  if (!tuim::database.query<tuim::Song>("SELECT * FROM songs WHERE path = ?;", file.string()).empty())
                    return;
                  tuim::FFmpeg::Tags tags = tuim::ffmpeg.get_tags(file.string());
                  tuim::database.execute("INSERT INTO songs VALUES (?, ?, ?, ?, ?, ?);", file.string(), tags.artist,
                                         tags.title, 0.0, 0.0, max_plays);
                });
  tuim::database.execute("COMMIT;");

  tuim::interface.song_menu.populate(
    tuim::database.query<tuim::Song>("SELECT * FROM songs ORDER BY LOWER(artist) ASC, LOWER(title) ASC;"));

  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::State::table.definition + ";");
  tuim::database.execute("INSERT OR IGNORE INTO state VALUES (?, ?, ?, ?);", 0, "", 0, 10);
}

void load_state()
{
  std::vector<tuim::State> state = tuim::database.query<tuim::State>("SELECT * FROM state;");
  if (state.empty())
  {
    std::cerr << "No state found!" << std::endl;
    exit(EXIT_FAILURE);
  }
  tuim::State &target_state = state.front();

  std::vector<tuim::Song> songs =
    tuim::database.query<tuim::Song>("SELECT * FROM songs WHERE path = ? LIMIT 1;", target_state.song_path.string());
  if (songs.empty()) return;
  tuim::Song &target_song = songs.front();

  tuim::player.play(target_song, false);
  tuim::player.seek_to(target_state.song_percentage);
  tuim::player.set_volume(target_state.volume_percentage);
}

int run_loop()
{
  for (ftxui::Loop loop = tuim::interface.create_loop(); !loop.HasQuitted();)
  {
    if (!tuim::player.music_active())
    {
      std::vector<tuim::Song> target_songs = {};
      target_songs = tuim::database.query<tuim::Song>(
        "SELECT * FROM songs WHERE plays < (SELECT MAX(plays) FROM songs) ORDER BY RANDOM() LIMIT 1;");
      if (target_songs.empty())
        target_songs = tuim::database.query<tuim::Song>("SELECT * FROM songs ORDER BY RANDOM() LIMIT 1;");
      if (target_songs.empty())
      {
        std::cerr << "No songs found!" << std::endl;
        return EXIT_FAILURE;
      }
      tuim::Song &target_song = target_songs.front();

      tuim::player.play(target_song);
    }

    std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now();
    while (tuim::player.music_active())
    {
      if (loop.HasQuitted()) break;
      if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_update).count() >= 3)
        tuim::database.execute("UPDATE state SET song_percentage = ? WHERE id = ?;",
                               static_cast<int>(std::round(tuim::player.progress_percentage)), 0);
      tuim::interface.screen.RequestAnimationFrame();
      loop.RunOnce();
    }
    tuim::player.unload();
  }

  return EXIT_SUCCESS;
}
