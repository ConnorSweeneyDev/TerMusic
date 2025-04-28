# TuiMusic
A terminal-based music player written using SDL, FTXUI and TagLib. It features:
- Pause/Unpause
- Seek
- Skip
- Volume Up/Down
- Search
- Playlists
- Shuffle
- Per-Song Volume Normalization Around -14dB
- Persistent State

## Usage
Playlists are in the form of folders on your filesystem - place absolute paths to folders in the `user/playlists.txt`
directory and the player will load them on startup. The player will start playing a random song from the first playlist
and will continue to pick random songs from that playlist after it finishes playing the current one. If you manually
change the playlist the player will start playing a random song from the new playlist instead. After adding a new MP3
file to an existing folder, you should run the player with the `-r` flag to re-cache the playlists and pick up the new
song.

https://github.com/user-attachments/assets/a1333cd2-cd4d-42e4-bba2-73056ddde9c6

> [!NOTE]
> Controls are as follows:
> - <kbd>j</kbd>, <kbd>k</kbd>, <kbd>down</kbd> or <kbd>up</kbd> to navigate the focused menu by 1 item.
> - <kbd>J</kbd> or <kbd>K</kbd> to navigate the focused menu by 12 items.
> - <kbd>T</kbd> or <kbd>B</kbd> to navigate to the top/bottom of the focused menu.
> - <kbd>h</kbd>, <kbd>l</kbd>, <kbd>left</kbd> or <kbd>right</kbd> to open/close the playlists tab.
> - <kbd>ctrl</kbd> + <kbd>f</kbd> to toggle search mode, type in the form `artist`, `title`, `artist|title` or
>   `title|artist`.
> - <kbd>return</kbd> to select the focused menu item.
> - <kbd>s</kbd> to shuffle the current/focused playlist.
> - <kbd>p</kbd> to pause/unpause.
> - <kbd>H</kbd> or <kbd>L</kbd> to seek forward/backward by 5%.
> - <kbd>0</kbd>, <kbd>1</kbd>... <kbd>9</kbd> to seek to 0%, 10%... 90% of the song.
> - <kbd>n</kbd> to end the current song.
> - <kbd>u</kbd> or <kbd>d</kbd> to increase/decrease the volume by 1%.
> - <kbd>U</kbd> or <kbd>D</kbd> to increase/decrease the volume by 5%.
> - <kbd>escape</kbd> to close the player.

# How to Build
This project is optimized to be built on Windows using MSVC.

1. Ensure that you have [MSVC](https://visualstudio.microsoft.com/downloads/) installed.
2. Ensure that you have [CMake](https://cmake.org/download/) installed, you can run `winget install Kitware.CMake` if
   you don't.
3. Ensure that you have [LLVM](https://releases.llvm.org/) installed, you can run `winget install LLVM.LLVM` and put the
   install location in your environment variables if you don't (for language server and clang-format support).
4. Execute `script/build.sh` followed by `script/run.sh`.

# How to Update Dependencies
All dependencies are vendored and stored in the `external` directory. Version information for dependencies can be found
in `external/version_info.txt`.

### SDL
1. Download the source code for the [release](https://github.com/libsdl-org/SDL/releases) you want.
2. Put the contents of the extracted folder in `external/SDL2`.
3. Put a copy of `external/SDL_mixer/include/SDL_mixer.h` in `external/SDL2/include`.

### SDL_mixer
1. Download the source code for the [release](https://github.com/libsdl-org/SDL_mixer/releases) you want.
2. Put the contents of the extracted folder in `external/SDL_mixer`.
3. Put a copy of `external/SDL_mixer/include/SDL_mixer.h` in `external/SDL2/include`.
4. Run `external/SDL_mixer/external/download.sh` and remove all git related files in each of the cloned folders.
5. Remove `external/SDL_mixer/.gitmodules`.

### FTXUI
1. Download the source code for the [release](https://github.com/ArthurSonzogni/FTXUI/releases) you want.
2. Put the contents of the extracted folder in `external/ftxui`.
3. Remove all git related files in the extracted folder.

### TagLib
1. Download the source code for the [release](https://github.com/taglib/taglib/releases) you want.
2. Download the source code for [utfcpp](https://github.com/nemtrif/utfcpp/releases) release you want.
3. Put the contents of the extracted folder in `external/taglib`.
4. Put the contents of the extracted utfcpp folder in `external/taglib/3rdparty/utfcpp`.
5. Remove all git related files in both extracted folders.
6. Remove the line `option(BUILD_SHARED_LIBS "Build shared libraries" OFF)` from `external/taglib/CMakeLists.txt`.
7. Move all `.h` and `.tcc` files from `external/taglib/taglib` to `external/taglib/include` recursively, ensuring that
   they are all on the same level.

### SQLite
1. Download the amalgamation for the [release](https://www.sqlite.org/download.html) you want.
2. Put the `sqlite3.c` and `sqlite3.h` files in `external/sqlite/source` and `external/sqlite/include/sqlite`
   respectively.

### FFmpeg
1. Download the pre-built [release](https://github.com/ffbinaries/ffbinaries-prebuilt/releases) for the version you
   want.
2. Put the executable in `external/ffmpeg`.
