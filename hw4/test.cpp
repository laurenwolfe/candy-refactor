#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {
    GameModel* b = new GameModel("json/test25by5.json");

    b->FreeArrays();
    delete b;

    return 0;
}