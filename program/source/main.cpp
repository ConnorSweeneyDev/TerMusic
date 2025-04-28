#include "main.hpp"

#include <cstdlib>
#include <iostream>

#include "SDL2/SDL_timer.h"

#include "player.hpp"

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++) std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    exit(1);
  }

  tuim::Player player;
  player.set_volume(20);
  player.load("C:/Users/conno/Music/Songs/The Connells - '74-'75.mp3");
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
