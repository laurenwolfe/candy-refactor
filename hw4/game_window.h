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
        "img/up_arrow.png",
        "img/left_arrow.png",
        "img/right_arrow.png",
        "img/down_arrow.png"
};

const char* candy_image_filenames[] = {
        "img/candy0.png",
        "img/candy1.png",
        "img/candy2.png",
        "img/candy3.png",
        "img/candy4.png",
        "img/candy5.png"
};
// END_GLOBALS ----------------------------------------------------

// NON-GTK_METHODS ------------------------------------------------
void make_window(GtkApplication* app);
void fill_window(GtkApplication *app);
void refresh_window(GtkApplication* app);
// END_NON-GTK_METHODS --------------------------------------------

#endif // _GAME_WINDOW_H_