# TerMusic
A terminal-based music player written using SDL, FTXUI, TagLib, SQLite and FFmpeg.

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
1. Download the pre-built [release](https://www.gyan.dev/ffmpeg/builds/) essentials for the version you want.
2. Put the executable in `external/ffmpeg`.
