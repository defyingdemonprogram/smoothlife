#!/bin/bash

set -xe

BUILD_DIR=build
CFLAGS="-Wall -Wextra -ggdb -O3"
INCLUDE="-I./raylib/raylib-5.5_linux_amd64/include"
LIBS="-Wl,-rpath=./raylib/raylib-5.5_linux_amd64/lib \
      -Wl,-rpath=$BUILD_DIR \
      -L./raylib/raylib-5.5_linux_amd64/lib \
      -l:libraylib.so -lm -ldl -lpthread"

mkdir -p "$BUILD_DIR"

# Compile smoothlife_term
cc $CFLAGS -o "$BUILD_DIR/smoothlife_term" smoothlife_term.c -lm

# Compile main
cc $CFLAGS -o "$BUILD_DIR/main" main.c $INCLUDE $LIBS

