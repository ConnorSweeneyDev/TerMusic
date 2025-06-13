#pragma once

#include <string>

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
    void update_info(Song &song);
    void update_plays(Song &song);
    void update_volume();

  public:
    Song current_song = {};
    float progress_percentage = 0.0f;
    std::string progress_text = "";
    int volume_percentage = 10;

  private:
    float volume_modifier = 1.0f;
    Mix_Music *music = nullptr;
  };

  inline Player player;
}
