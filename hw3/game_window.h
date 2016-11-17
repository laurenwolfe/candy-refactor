#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array2d.h>
#include "model.h"

// GLOBALS --------------------------------------------------------
GtkApplication* app;
static BoardPtr gameboard;
char *PATH = "img/";

static char* dbutton_image_filenames[] = {
        PATH + "up_arrow.png",
        PATH + "left_arrow.png",
        PATH + "right_arrow.png",
        PATH + "down_arrow.png"
};
static char* candy_image_filenames[] = {
        PATH + "candy0.png",
        PATH + "candy1.png",
        PATH + "candy2.png",
        PATH + "candy3.png",
        PATH + "candy4.png",
        PATH + "candy5.png"
};
// END_GLOBALS ----------------------------------------------------

// NON-GTK_METHODS ------------------------------------------------
void fill_window(GtkApplication *app);
// END_NON-GTK_METHODS --------------------------------------------
#endif // _GAME_WINDOW_H_