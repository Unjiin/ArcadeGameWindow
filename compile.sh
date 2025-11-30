#!/bin/bash
gcc main.c apps/*.c utils/*.c -o ArcadeGame.exe \
    $(pkg-config --cflags gtk4 gstreamer-1.0) \
    $(pkg-config --libs gtk4 gstreamer-1.0) \
    -mwindows && ./ArcadeGame.exe

# cd /c/Users/1/CLionProjects/ArcadeGame