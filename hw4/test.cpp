#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {
    GameModel* b = new GameModel("test25by5.json");

    b->PrintBoard();

    b->SerializeGame("output.json");

    return 0;
}