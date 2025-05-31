#!/bin/bash

LEVEL=0
DATABASES=1

if [ $LEVEL == 0 ]; then
  rm -rf build/TerMusic.dir
elif [ $LEVEL == 1 ]; then
  rm -rf build/TerMusic.dir
  rm -rf build/external
  rm -rf vcpkg_installed
elif [ $LEVEL == 2 ]; then
  rm -rf build
  rm -rf vcpkg_installed
fi

if [ $DATABASES == 1 ]; then
  rm -rf user/database/*.db
fi
