#!/bin/bash

LEVEL=0
# LEVEL=1
# LEVEL=2
DATABASE=0
# DATABASE=1

if [ $LEVEL == 0 ]; then
  rm -rf build/TerMusic.dir
elif [ $LEVEL == 1 ]; then
  rm -rf build/TerMusic.dir build/_deps build/cmake build/vcpkg
elif [ $LEVEL == 2 ]; then
  rm -rf build
fi

if [ $DATABASE == 1 ]; then
  rm -rf user/*.db
fi
