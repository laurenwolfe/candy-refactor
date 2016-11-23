#include <iostream>
#include "game_model.h"

using namespace std;

int main(int argc, char **argv) {

    GameModel b = GameModel("test25by5.json");

    cout << "Moves remaining: " << b.GetMovesRemaining() << endl;
    cout << "Score: " << b.GetScore() << endl;
    b.PrintBoard();

    b.SetSelectedCandy(12);
    cout << "Selected:" + to_string(b.GetCandyColor(12));
    cout << " at: (row:" + to_string(b.ConvertToRow(12));
    cout << ",col:" + to_string(b.ConvertToRow(12)) + ")\n";
    if(b.SwapCandy('W')) {
        cout << "W Success!" << endl;
        b.PrintBoard();
        cout << "Moves remaining: " << b.GetMovesRemaining() << endl;
        cout << "Score: " << b.GetScore() << endl;
    } else {
        cout << "W fail!" << endl;
    }
    b.SetSelectedCandy(18);
    cout << "Selected:" + to_string(b.GetCandyColor(18));
    cout << " at: (row:" + to_string(b.ConvertToRow(18));
    cout << ",col:" + to_string(b.ConvertToRow(18)) + ")\n";
    if(b.SwapCandy('S')) {
      cout << "S Success!" << endl;
      b.PrintBoard();
      cout << "Moves remaining: " << b.GetMovesRemaining() << endl;
      cout << "Score: " << b.GetScore() << endl;
    } else {
      cout << "S fail!" << endl;
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