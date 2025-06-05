#include "player.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "SDL2/SDL.h"
#include "SDL2/SDL_error.h"
#include "SDL2/SDL_main.h"
#include "SDL2/SDL_mixer.h"

#include "database.hpp"
#include "ffmpeg.hpp"
#include "song.hpp"

namespace tuim
{
  Player::Player()
  {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
      std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
      exit(1);
    }
    if (Mix_Init(MIX_INIT_MP3) == 0)
    {
      std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) != 0)
    {
      std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
    if (volume_percentage < 0) volume_percentage = 0;
    if (volume_percentage > 100) volume_percentage = 100;
  }

  Player::~Player()
  {
    Mix_FreeMusic(music);
    music = nullptr;
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
  }

  void Player::play(Song &song)
  {
    music = Mix_LoadMUS(song.path.string().c_str());
    if (music == nullptr)
    {
      std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }

    if (song.mean_volume == 0.0)
    {
      tuim::ffmpeg.update_mean_volume(song.path.string());
      song.mean_volume = tuim::ffmpeg.last_mean_volume;
      tuim::database.execute("UPDATE " + tuim::Song::table.name + " SET mean_volume = ? WHERE path = ?;",
                             song.mean_volume, song.path.string());
    }
    volume_modifier = static_cast<float>(song.mean_volume) / -14.0f;
    update_volume();

    if (Mix_PlayMusic(music, 0) != 0)
    {
      std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
    song.plays++;
    tuim::database.execute("UPDATE " + tuim::Song::table.name + " SET plays = ? WHERE path = ?;", song.plays,
                           song.path.string());
  }

  void Player::unload()
  {
    Mix_FreeMusic(music);
    music = nullptr;
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

  void Player::change_volume(const int &delta)
  {
    volume_percentage += delta;
    if (volume_percentage < 0) volume_percentage = 0;
    if (volume_percentage > 100) volume_percentage = 100;
    update_volume();
  }

  void Player::update_volume()
  {
    float real_volume =
      std::round((static_cast<float>(volume_percentage) * (MIX_MAX_VOLUME / 100.0f)) * volume_modifier);
    if (real_volume > MIX_MAX_VOLUME) real_volume = MIX_MAX_VOLUME;
    if (real_volume < 0) real_volume = 0;
    Mix_VolumeMusic(static_cast<int>(real_volume));
  }
}
