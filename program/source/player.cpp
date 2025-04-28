#include "player.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_error.h"
#include "SDL2/SDL_main.h"
#include "SDL2/SDL_mixer.h"

namespace tuim
{
  Player::Player()
  {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
      std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
      exit(1);
    }
    if (Mix_Init(MIX_INIT_MP3) == 0)
    {
      std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) != 0)
    {
      std::cout << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  Player::~Player()
  {
    Mix_FreeMusic(music);
    music = nullptr;
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
  }

  void Player::load(const std::filesystem::path &path)
  {
    music = Mix_LoadMUS(path.string().c_str());
    if (music == nullptr)
    {
      std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void Player::unload()
  {
    Mix_FreeMusic(music);
    music = nullptr;
  }

  void Player::play()
  {
    if (Mix_PlayMusic(music, 0) != 0)
    {
      std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void Player::toggle_pause()
  {
    if (Mix_PausedMusic())
      Mix_ResumeMusic();
    else
      Mix_PauseMusic();
  }

  bool Player::music_active()
  {
    if (Mix_PlayingMusic())
      return true;
    else
      return false;
  }

  void Player::set_volume(const int &volume) { Mix_VolumeMusic(volume); }
}
