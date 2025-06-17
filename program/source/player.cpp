#include "player.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL_error.h"
#include "SDL2/SDL_main.h"
#include "SDL2/SDL_mixer.h"

#include "database.hpp"
#include "ffmpeg.hpp"
#include "interface.hpp"
#include "song.hpp"
#include "utility.hpp"

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
    unload();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
  }

  void Player::play(Song &song)
  {
    unload();

    std::vector<Song> results = {};
    if (results = database.query<Song>("SELECT * FROM songs WHERE path = ? LIMIT 1;", song.path.string());
        results.empty())
    {
      std::cerr << "Song not found in database!" << std::endl;
      exit(EXIT_FAILURE);
    }
    Song target_song = results.front();

    music = Mix_LoadMUS(target_song.path.string().c_str());
    if (!music)
    {
      std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }

    update_info(target_song);
    update_volume();
    interface.song_menu.move_to(target_song);

    if (Mix_PlayMusic(music, 0) != 0)
    {
      std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
      exit(EXIT_FAILURE);
    }
    update_plays(target_song);

    current_song = target_song;
    paused = false;
  }

  void Player::unload()
  {
    if (!music) return;
    Mix_FreeMusic(music);
    music = nullptr;
  }

  void Player::toggle_pause()
  {
    if (Mix_PausedMusic())
    {
      Mix_ResumeMusic();
      paused = false;
      return;
    }

    Mix_PauseMusic();
    paused = true;
  }

  bool Player::music_active()
  {
    if (Mix_PlayingMusic())
    {
      double current_seconds = Mix_GetMusicPosition(music);
      double total_seconds = Mix_MusicDuration(music);
      progress_percentage = static_cast<float>(round(current_seconds / total_seconds * 100.0));
      if (progress_percentage < 0.0f) progress_percentage = 0.0f;
      if (progress_percentage > 100.0f) progress_percentage = 100.0f;
      progress_text = utility::seconds_to_string(current_seconds);
      return true;
    }
    else
      return false;
  }

  void Player::seek_to(const int &percentage)
  {
    if (music_active()) Mix_SetMusicPosition(Mix_MusicDuration(music) * (static_cast<double>(percentage) / 100.0));
  }

  void Player::seek_by(const int &percentage_delta)
  {
    if (music_active())
    {
      if (percentage_delta < 0)
        if (progress_percentage < static_cast<float>(abs(percentage_delta)))
        {
          Mix_SetMusicPosition(0.0);
          return;
        }
      if (percentage_delta > 0)
        if (progress_percentage > (100.0f - static_cast<float>(abs(percentage_delta))))
        {
          Mix_SetMusicPosition(Mix_MusicDuration(music));
          return;
        }
      Mix_SetMusicPosition(Mix_GetMusicPosition(music) +
                           (Mix_MusicDuration(music) * (static_cast<double>(percentage_delta) / 100.0)));
    }
  }

  void Player::change_volume(const int &percentage_delta)
  {
    volume_percentage += percentage_delta;
    if (volume_percentage < 0) volume_percentage = 0;
    if (volume_percentage > 100) volume_percentage = 100;
    update_volume();
  }

  void Player::update_info(Song &song)
  {
    if (song.duration == 0.0)
    {
      song.duration = Mix_MusicDuration(music);
      database.execute("UPDATE songs SET duration = ? WHERE path = ?;", song.duration, song.path.string());
    }

    if (song.mean_volume == 0.0)
    {
      ffmpeg.update_mean_volume(song.path.string());
      song.mean_volume = ffmpeg.last_mean_volume;
      database.execute("UPDATE songs SET mean_volume = ? WHERE path = ?;", song.mean_volume, song.path.string());
    }
    volume_modifier = static_cast<float>(song.mean_volume) / -14.0f;
    if (volume_modifier < 0.0f) volume_modifier = abs(volume_modifier);
    if (volume_modifier == 0.0f) volume_modifier = 1.0f;
  }

  void Player::update_plays(Song &song)
  {
    if (std::vector<Song> results =
          database.query<Song>("SELECT * FROM songs WHERE path != ? ORDER BY plays ASC LIMIT 1;", song.path.string());
        !results.empty())
    {
      if (song.plays > results.front().plays) return;
      song.plays++;
      database.execute("UPDATE " + Song::table.name + " SET plays = ? WHERE path = ?;", song.plays, song.path.string());
    }
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
