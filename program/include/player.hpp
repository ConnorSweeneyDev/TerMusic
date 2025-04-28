#pragma once

#include <filesystem>

#include "SDL2/SDL_mixer.h"

namespace tuim
{
  class Player
  {
  public:
    Player();
    ~Player();

    void load(const std::filesystem::path &path);
    void unload();
    void play();
    void toggle_pause();
    bool music_active();
    void set_volume(const int &new_volume);

  private:
    Mix_Music *music = nullptr;
  };
}
