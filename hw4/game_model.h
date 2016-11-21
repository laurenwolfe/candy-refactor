#ifndef _GAME_MODEL_H
#define _GAME_MODEL_H_

#include <vector>
#include <string>

extern "C" {
    // #include <array2d.h>
    // #include <jansson.h>

    // temporary includes to make visual studio happy
    // comment out if discovered otherwise
    #include "../jansson/include/jansson.h"
    #include "../hw2/array2d.h"
}

using namespace std;

typedef struct candy {
    int color;
    int type = 0;
} Candy, *CandyPtr;

class GameModel {
    public:
        // --- Constructors ---
        GameModel();
        GameModel(const string &filepath);

        int GetScore() const { return score_; };
        int GetGameID() const { return game_id_; };

        int GetMovesRemaining() const;
        int GetMaxScore() const;
        int GetSelectedCandyIdx() const;
        CandyPtr GetSelectedCandyPtr() const;
        int GetCandyColor(const int &idx) const;
        int GetBoardSize() const;
        int GetRowLength() const;
        int GetColLength() const;
        int ConvertToRow(const int &idx) const;
        int ConvertToCol(const int &idx) const;
        int ConvertToIdx(const int &row, const int &col) const;
        int GetBoardSize() const;
        void PrintBoard();
        bool IsGameOver();

        void SetSelectedCandy(int idx);
        bool SwapCandy(const char &dir);

    private:
        // --- Deserialize Functions ---
        bool DeserializeGameInstance(const char* &filepath);
        bool DeserializeGameDef(json_t* game_instance);
        int DeserializeGameState(json_t* game_instance);
        Array2D DeserializeArray2D(json_t* serialized_array2d, ElDeserializeFnPtr deserialize_function);

        // --- Serialize Functions ---
        void SerializeGame(const string &filepath);
        bool SerializeGameInstance(const char* &filepath);
        json_t* SerializeGameDef(void);
        json_t* SerializeGameState(void);
        json_t* SerializeArray2D(Array2D array, ElSerializeFnPtr serialize_function);

        // --- Private Functions ---
        void SetCandy(const int &idx, const int &color, const int &type);
        void SetCandy(const int &dest_idx, const int &source_idx);
        bool FreeCandy(const int &idx);

        bool TrySwap(const int &idx1, const int &idx2);
        bool HasVerticalMatch(const int &idx);
        bool HasHorizontalMatch(const int &idx);
        bool ScanSequence(const int size, vector<int> candy_seq);

        bool FireBoardLoop();
        bool FindAndFireTemplates(const int &num, const bool &isVertical);
        bool FireTemplate(int idx, const int &num, const int &increment);
        void ApplyGravity();
        void FillFromExtensionBoard();

        void AdjustScore(const int &idx); //if fires remaining, inc score and dec fires

        // --- Class Instance Variables --- 
        Array2D extension_board_; //extra candy to drop (extensioncolor)
        Array2D game_board_; //current visible board (boardcandies)
        Array2D original_fired_state_; //fired state as initally read from gamedef (boardstate)
        Array2D fired_state_; //how many times each cell needs to fire before win (boardstate)
        vector<int> extension_offset_; //row number for next drop position of extension board (extensionoffset)
        int sel_candy_idx_ = -1; //button-selected candy
        int game_id_; // (gameid)
        int total_moves_; // (movesallowed)
        int moves_made_; // (movesmade)
        int num_colors_; //number of colors for this game board (colors)
        int max_score_;
        int score_ = 0; // (currentscore)

        // --- Class Constants ---
        const int NO_CANDY = -1;
        const int MIN_MATCH_LENGTH = 3;
        const int DEFAULT_CANDY_TYPE = 0;
        const int MAX_SETTLE = 1000;
        const Candy EMPTY_CANDY = {NO_CANDY, 0};
};

#endif // _GAME_MODEL_H_
