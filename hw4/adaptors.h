#ifndef _ADAPTORS_H_
#define _ADAPTORS_H_

#include "game_model.h"

GameModel* CreateGameModel(char *filepath);
void FreeModel(GameModel *gm);

#endif