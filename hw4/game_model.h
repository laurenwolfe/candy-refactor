#ifndef _GAME_MODEL_H
#define _GAME_MODEL_H_

#include <vector>
#include <string>

extern "C" {
    #include <array2d.h>
    #include <jansson.h>
}

using namespace std;

typedef struct candy {
    int color;
    int type = 0;
} Candy, *CandyPtr;

class GameModel {
    public:
        GameModel();
        GameModel(string filepath);

        int GetScore() const { return score_; };
        int GetGameID() const { return game_id_; };

        int GetMovesRemaining() const;
        int GetSelectedCandyIdx() const;
        CandyPtr GetSelectedCandyPtr() const;
        int GetCandyColor(const int &idx) const;
        int GetRowLength() const;
        int GetColLength() const;
        int ConvertToRow(const int &idx) const;
        int ConvertToCol(const int &idx) const;
        int ConvertToIdx(const int &row, const int &col) const;
        int GetBoardSize() const;
        void PrintBoard();

        void SetSelectedCandy(int idx);
        bool SwapCandy(const char &dir);
        bool IsGameOver();

    private:
        void DeserializeGame(const string &filepath);
        void SerializeGame(const string &filepath);

        bool TrySwap(const int &idx1, const int &idx2);
        bool HasVerticalMatch(const int &idx);
        bool HasHorizontalMatch(const int &idx);
        bool ScanSequence(const int size, vector<int> candy_seq);
        void SetColor(const int &idx, const int &value);
        bool FireBoardLoop();
        bool FindAndFireVerticalMatch(const int &num);
        bool FindAndFireHorizontalMatch(const int &num);
        void AdjustScore(const int &idx); //if fires remaining, inc score and dec fires
        void ApplyGravity();
        void FillFromExtensionBoard();

        Array2D extension_board_; //extra candy to drop
        Array2D game_board_; //current visible board
        Array2D fired_state_; //how many times each cell needs to fire before win
        vector<int> extension_offset_; //row number for next drop position of extension board
        int sel_candy_idx_ = -1; //button-selected candy
        int game_id_;
        int total_moves_;
        int moves_made_;
        int num_colors_; //number of colors for this game board
        int score_ = 0;
        const int NO_CANDY = -1;
        const int MIN_MATCH_LENGTH = 3;
};

#endif // _GAME_MODEL_H_
