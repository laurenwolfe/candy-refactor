#include "adaptors.h"

GameModel* CreateGameModel(char *filepath) {
    string file(filepath);

    GameModel *gm = new GameModel(file);
    gm->PrintBoard();
    return gm;
}

void FreeModel(GameModel *gm) {
    gm->FreeArrays();
    delete gm;
}