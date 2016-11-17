#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hw2/array2d.h"
#include "model.h"

// GLOBALS --------------------------------------------------------
GtkApplication* app;
static BoardPtr gameboard;

static char* dbutton_image_filenames[] = {
        "up_arrow.png",
        "left_arrow.png",
        "right_arrow.png",
        "down_arrow.png"
};

static char* candy_image_filenames[] = {
        "candy0.png",
        "candy1.png",
        "candy2.png",
        "candy3.png",
        "candy4.png",
        "candy5.png"
};
// END_GLOBALS ----------------------------------------------------

// NON-GTK_METHODS ------------------------------------------------
void make_window(GtkApplication* app);
void fill_window(GtkApplication *app);
void refresh_window(GtkApplication* app);
// END_NON-GTK_METHODS --------------------------------------------

#endif // _GAME_WINDOW_H_
