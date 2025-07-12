#!/bin/bash

GENERATOR="Visual Studio 17 2022"
TYPE="Release"
# TYPE="Debug"

cmake -S . -B build -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$TYPE"
cmake --build build --config "$TYPE"
