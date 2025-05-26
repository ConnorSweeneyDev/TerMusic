#!/bin/bash

TYPE=Release

cmake -S . -B build -DCMAKE_BUILD_TYPE=$TYPE
cmake --build build --config $TYPE
