#ifndef _GAME_MODEL_H
#define _GAME_MODEL_H_

#include <vector>
#include <string>

extern "C" {
    #include <array2d.h>
    #include <jansson.h>
    #include "../hw3/model.h"
}

using namespace std;

struct Candy {
    int color;
    int type = 0;
};

class Board {
    public:
        Board();
        Board(string filepath);

        int GetMovesRemaining() const { return moves_left_; };
        int GetScore() const { return score_; };
        int GetGameID() const { return game_id_; };

        Candy GetSelectedCandy() const;
        int GetRowLength() const;
        int GetColLength() const;
        int GetBoardSize() const;
        bool SwapCandy(const int &idx1, const int &idx2);
        bool IsGameOver();

    private:
        void DeserializeGame(const string &filepath);
        //void DeserializeFunction(Array2D array, Json_ptr data);
        void SerializeGame(const string &filepath);

        bool IsValidSwap(const int &idx1, const int &idx2);
        void FireBoardLoop();
        void FindMatches();
        void FindVerticalMatches(const int &num);
        void FindHorizontalMatches(const int &num);
        void AdjustScore(); //if fires remaining, inc score and dec fires
        void ApplyGravity();
        void FillFromExtensionBoard();

        Array2D extension_board_; //extra candy to drop
        Array2D game_board_; //current visible board
        Array2D fired_state_; //how many times each cell needs to fire before win
        vector<int> extension_offset_; //row number for next drop position of extension board
        Candy sel_candy_; //button-selected candy -- TODO: need to delete after swap
        int game_id_;
        int total_moves_;
        int moves_left_;
        int num_colors_; //number of colors for this game board
        int score_ = 0;
};

#endif // _GAME_MODEL_H_
