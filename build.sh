#!/bin/bash

set -xe

SRC_DIR=src
BUILD_DIR=build

CFLAGS="-Wall -Wextra -ggdb -O3"
INCLUDE="-I./raylib/raylib-5.5_linux_amd64/include"
LIBS="-Wl,-rpath=./raylib/raylib-5.5_linux_amd64/lib \
      -Wl,-rpath=$BUILD_DIR \
      -L./raylib/raylib-5.5_linux_amd64/lib \
      -l:libraylib.so -lm -ldl -lpthread"

mkdir -p "$BUILD_DIR"

# Compile smoothlife_term
cc $CFLAGS -o "$BUILD_DIR/smoothlife_term" $SRC_DIR/smoothlife_term.c -lm

# Compile smoothlife_gui
cc $CFLAGS -o "$BUILD_DIR/smoothlife_gui" $SRC_DIR/smoothlife_gui.c $INCLUDE $LIBS

# Compile smoothlife_gpu
cc $CFLAGS -o "$BUILD_DIR/smoothlife_gpu" $SRC_DIR/smoothlife_gpu.c $INCLUDE $LIBS

