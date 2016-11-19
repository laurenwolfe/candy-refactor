#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {
    GameModel b;
    //SetSelectedCandy(int idx)
    //bool SwapCandy(char dir)
    b.SetSelectedCandy(5);
    cout << "South:" << endl;
    cout << "Result: " << b.SwapCandy('S') << endl;

    b.SetSelectedCandy(5);
    cout << "West:" << endl;
    cout << b.SwapCandy('W') << endl;

    b.SetSelectedCandy(5);
    cout << "North:" << endl;
    cout << b.SwapCandy('N') << endl;

    b.SetSelectedCandy(5);
    cout << "East:" << endl;
    cout << b.SwapCandy('E') << endl;

    //Test cases: swap idx 0 = S, W, E = false, N = true
    // swap idx 15 = E = true, W, N, S = false
    // swap idx 7 = E = true, W, N, S = false
    // swap idx 5 = all false

    /*
     5, 2, 1, 0,
     0, 5, 4, 0,
     3, 5, 3, 3,
     0, 2, 1, 0
     */

    return 0;
}