#ifndef _GAME_MODEL_H
#define _GAME_MODEL_H_

#include <vector>;

extern "C" {
    include <array2d.h>;
    include <jansson.h>;
};

using namespace std;

struct Candy {
    int color;
    int type = 0;
};

struct Game {

};

class Board {
    public:
        Board(string filepath);
        boolean SwapCandy();

        Candy GetSelectedCandy() const { return game_data.sel_candy; };
        int GetMovesRemaining const { return moves_left_; };
        int GetRowLength();
        int GetColLength();
        int GetBoardSize();

    private:
        boolean IsValidSwap(int idx1, int idx2);
        void SerializeGame(string filepath);
        void DeserializeGame(string filepath);

        const Array2D extension_board_; //extra candy to drop
        Array2D game_board_; //current visible board
        Array2D fired_state_; //how many times each cell needs to fire before win
        vector<int> extension_offset_; //row number for next drop position of extension board
        Candy sel_candy_; //button-selected candy -- TODO: need to delete after swap
        const int game_id_;
        const int total_moves_;
        int moves_left_;
        const int num_colors_; //number of colors for this game board
        int score_ = 0;

};

#endif // _GAME_MODEL_H_
