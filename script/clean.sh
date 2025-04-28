#!/bin/bash

LEVEL=0

if [ $LEVEL == 0 ]; then
  rm -rf build/TerMusic.dir
elif [ $LEVEL == 1 ]; then
  rm -rf build/TerMusic.dir
  rm -rf build/external
elif [ $LEVEL == 2 ]; then
  rm -rf build
fi
