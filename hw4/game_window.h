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
#include <vector>

using std::vector;

class GameWindow {
public:
  static GtkApplication* app;

private:
  const vector<string> dbutton_image_filenames {
    "up_arrow.png", 
    "left_arrow.png",
    "right_arrow.png",
    "down_arrow.png"
  };
  const vector<string> candy_image_filenames {
    "candy0.png",
    "candy1.png",
    "candy2.png",
    "candy3.png",
    "candy4.png",
    "candy5.png"
  };
  const vector<char> direction_chars{'N', 'W', 'E', 'S'};
  GameModel gameboard;

  // METHODS
  void make_window(GtkApplication* app);
  void fill_window(GtkApplication* app);
  void refresh_window(GtkApplication* app);
};

#endif // _GAME_WINDOW_CPP_H_
