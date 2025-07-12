# TerMusic
A terminal-based music player written using SDL, FTXUI, SQLite and FFmpeg.

https://github.com/user-attachments/assets/6aa83dd1-62f9-4e4e-8a70-e93c1f346aa3

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
All dependencies are managed by either [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) or
[VCPKG](https://github.com/microsoft/vcpkg). After changing the version of a dependency, do a full clean build using
`script/clean.sh` before `script/build.sh`.

### [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake/releases) and [VCPKG](https://github.com/microsoft/vcpkg/releases)
Change the `[MANAGER]_VERSION` variable inside `cmake/Manager.cmake` to the desired version.

### [SDL](https://github.com/libsdl-org/SDL/releases), [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer/releases), [FTXUI](https://github.com/ArthurSonzogni/FTXUI/releases) and [SQLite](https://www.sqlite.org/download.html)
Change the `[DEPENDENCY]_VERSION` variable inside `cmake/External.cmake` to the desired version.

### [FFmpeg](https://github.com/FFmpeg/FFmpeg/tags)
Change the `version` field inside the `ffmpeg` override in `vcpkg.json` to the desired version.
