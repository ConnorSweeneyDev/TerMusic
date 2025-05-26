#include "main.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "SDL2/SDL_timer.h"
#include "nlohmann/json.hpp"

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

  std::filesystem::path path = "C:/Users/conno/Music/Songs/The Connells - '74-'75.mp3";

  std::string command =
    "external\\ffmpeg\\ffprobe.exe -v quiet -print_format json -show_format \"" + path.string() + "\"";
  std::string output = run_command(command);
  nlohmann::json metadata = nlohmann::json::parse(output);
  std::string title = metadata["format"]["tags"]["title"].get<std::string>();
  std::string artist = metadata["format"]["tags"]["artist"].get<std::string>();

  database.execute("INSERT OR IGNORE INTO songs (path, title, artist) VALUES (?, ?, ?);",
                   {path.string(), title, artist});

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

std::string run_command(const std::string &command)
{
  std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
  if (!pipe)
  {
    std::cerr << "Failed to run command: " << command << std::endl;
    exit(1);
  }

  std::array<char, 256> buffer;
  std::string result;
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) { result += buffer.data(); }
  return result;
}
