#!/bin/bash
gcc main.c appMenu.c appMinesweeper.c utils.c appSettings.c musicUtils.c imageUtils.c -o ArcadeGame.exe \
    $(pkg-config --cflags gtk4 gstreamer-1.0) \
    $(pkg-config --libs gtk4 gstreamer-1.0) \
    -mwindows && ./ArcadeGame.exe