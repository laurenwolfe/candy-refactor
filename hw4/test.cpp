#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {
    GameModel b;

    b.SetSelectedCandy(11);
    if(b.SwapCandy('N')) {
        cout << "Success!" << endl;
        b.PrintBoard();
        cout << "Moves remaining: " << b.GetMovesRemaining() << endl;
        cout << "Score: " << b.GetScore() << endl;
    } else {
        cout << "fail!" << endl;
    }


//    b.SetSelectedCandy(15);
//    b.SwapCandy('E');



    /*
    b.SetSelectedCandy(7);
    b.SwapCandy('E');
    */

    //Test cases: swap idx 0 = S, W, E = false, N = true
    // swap idx 15 = E = true, W, N, S = false
    // swap idx 7 = E = true, W, N, S = false
    // swap idx 5 = all false

    return 0;
}