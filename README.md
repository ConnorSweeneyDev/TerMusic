# TerMusic
A terminal-based music player written using SDL, FTXUI, SQLite and FFmpeg.

# How to Use
1. Download the latest [release](https://github.com/ConnorSweeneyDev/TerMusic/releases).
2. Extract the contents of the archive to a directory of your choice.
3. Modify the `user/playlist.txt` file to point to the directory containing your mp3 files.
4. Execute `TerMusic.exe`.

> [!NOTE]
> Controls are as follows:
> - <kbd>j</kbd>, <kbd>k</kbd> to navigate by 1 item.
> - <kbd>ctrl</kbd> + <kbd>d</kbd>, <kbd>ctrl</kbd> + <kbd>u</kbd> to navigate by 12 items.
> - <kbd>g</kbd>, <kbd>G</kbd> to navigate to the top/bottom.
> - <kbd>/</kbd> to enable search mode (searches using substrings).
> - <kbd>return</kbd> to apply the search or, if not searching, to play the hovered song.
> - <kbd>esc</kbd> to clear the search and disable search mode.
> - <kbd>p</kbd> to pause/unpause.
> - <kbd>h</kbd>, <kbd>l</kbd> to seek -5%/+5% of the current song.
> - <kbd>0</kbd>, <kbd>1</kbd>... <kbd>9</kbd> to seek to 0%, 10%... 90% of the song.
> - <kbd>n</kbd> to skip the current song.
> - <kbd>u</kbd>, <kbd>d</kbd> to change the volume by 1%.
> - <kbd>U</kbd>, <kbd>D</kbd> to change the volume by 5%.
> - <kbd>q</kbd> to close the player.

# How to Build
This project is optimized to be built on Windows using MSVC.

1. Ensure that you have [MSVC](https://visualstudio.microsoft.com/downloads/) installed.
2. Ensure that you have [CMake](https://cmake.org/download/) installed, you can run `winget install Kitware.CMake` if
   you don't.
3. Ensure that you have [LLVM](https://releases.llvm.org/) installed, you can run `winget install LLVM.LLVM` and put the
   install location in your environment variables if you don't (for language server and clang-format support).
4. Execute `script/build.sh` followed by `script/run.sh`.

# How to Update Dependencies
All dependencies are vendored and either stored in the `external` directory or pulled in by vcpkg. Version information
for dependencies can be found in `external/version_info.txt`.

### SDL
1. Download the source code (light) for the [release](https://github.com/libsdl-org/SDL/releases) you want.
2. Put the contents of the extracted folder in `external/SDL2`.
3. Put a copy of `external/SDL_mixer/include/SDL_mixer.h` in `external/SDL2/include`.

### SDL_Mixer
1. Download the source code (light) for the [release](https://github.com/libsdl-org/SDL_mixer/releases) you want.
2. Put the contents of the extracted folder in `external/SDL_mixer`.
3. Put a copy of `external/SDL_mixer/include/SDL_mixer.h` in `external/SDL2/include`.
4. Remove `external/SDL_mixer/.gitmodules`.

### FTXUI
1. Download the source code for the [release](https://github.com/ArthurSonzogni/FTXUI/releases) you want.
2. Put the contents of the extracted folder in `external/ftxui`.
3. Remove all git related files in the extracted folder.

### SQLite
1. Download the amalgamation for the [release](https://www.sqlite.org/download.html) you want.
2. Put the `sqlite3.c` and `sqlite3.h` files in `external/sqlite/source` and `external/sqlite/include/sqlite`
   respectively.

### VCPKG
1. Modify the cmake variable `VCPKG_RELEASE` to be the desired [tag](https://github.com/microsoft/vcpkg/releases).
2. Modify the `vcpkg.json` manifest's `builtin-baseline` default to point to the commit of the tag you just chose.

### FFmpeg
1. Modify the `vcpkg.json` manifest's `ffmpeg` override to point to the desired
   [FFmpeg](https://ffmpeg.org/download.html#releases) release.
