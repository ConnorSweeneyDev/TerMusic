#pragma once

#include "SDL2/SDL_mixer.h"

#include "song.hpp"

namespace tuim
{
  class Player
  {
  public:
    Player();
    ~Player();

    void play(Song &song);
    void unload();
    void toggle_pause();
    bool music_active();
    void change_volume(const int &delta);

  private:
    void update_volume();

  public:
    int current_progress = 0;
    int volume_percentage = 10;

  private:
    float volume_modifier = 1.0f;
    Mix_Music *music = nullptr;
  };

  inline Player player;
}
