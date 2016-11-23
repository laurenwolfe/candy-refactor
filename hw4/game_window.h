#ifndef _GAME_WINDOW_CPP_H_
#define _GAME_WINDOW_CPP_H_

extern "C" {
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array2d.h>
}

#include "game_model.h"

static class GameWindow {
  // GLOBALS --------------------------------------------------------
  GtkApplication* app;
  static GameModel gameboard;
public:
  static string dbutton_image_filenames[] = {
    "up_arrow.png",
    "left_arrow.png",
    "right_arrow.png",
    "down_arrow.png"
  };

  static string candy_image_filenames[] = {
    "candy0.png",
    "candy1.png",
    "candy2.png",
    "candy3.png",
    "candy4.png",
    "candy5.png"
  };
  // END_GLOBALS ----------------------------------------------------

  // NON-GTK_METHODS ------------------------------------------------
  static void make_window(GtkApplication* app);
  void fill_window(GtkApplication *app);
  void refresh_window(GtkApplication* app);
  // END_NON-GTK_METHODS --------------------------------------------

};

#endif // _GAME_WINDOW_CPP_H_
