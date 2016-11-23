#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_

extern "C" {
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array2d.h>
}

#include "adaptors.h"

// GLOBALS --------------------------------------------------------
GtkApplication* app;
GameModel *gameboard;

const char* dbutton_image_filenames[] = {
        "images/direction/up.png",
        "images/direction/left.png",
        "images/direction/right.png",
        "images/direction/down.png"
};

const char* candy_image_filenames[] = {
        "images/40x40/blue.png",
        "images/40x40/green.png",
        "images/40x40/orange.png",
        "images/40x40/purple.png",
        "images/40x40/red.png",
        "images/40x40/yellow.png"
};
// END_GLOBALS ----------------------------------------------------

// NON-GTK_METHODS ------------------------------------------------
void make_window(GtkApplication* app);
void fill_window(GtkApplication *app);
void refresh_window(GtkApplication* app);
// END_NON-GTK_METHODS --------------------------------------------

#endif // _GAME_WINDOW_H_