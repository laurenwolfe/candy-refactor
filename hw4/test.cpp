#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {
    GameModel* b = new GameModel("gameExample.json");

    b->PrintBoard();

    b->SerializeGame("output2.json");

    return 0;
}