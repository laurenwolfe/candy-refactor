#ifndef _GAME_MODEL_H
#define _GAME_MODEL_H_

#include <vector>
#include <string>
#include <cstring> // for memcpy

extern "C" {
    #include <array2d.h>
    #include <jansson.h>
}

using namespace std;

typedef struct candy {
    candy() : color(-1), type(0) {}
    int color;
    int type = 0;
} Candy, *CandyPtr;

class GameModel {
    public:
        GameModel(const string &filepath);

        // --- Getters ---
        int GetScore() const { return score_; };
        int GetGameID() const { return game_id_; };
        int GetMovesRemaining() const;
        int GetMaxScore() const;
        int GetSelectedCandyIdx() const;
        int GetCandyColor(const int &idx) const;
        int GetBoardSize() const;
        int GetRowLength() const;
        int GetColLength() const;

        // --- Other Public Methods ---
        void PrintBoard();
        void PrintBoard(const Array2D &board);
        bool IsGameOver();
        void SetSelectedCandy(int idx);
        bool SwapCandy(const char &dir);
        void SerializeGame(const string &filepath);

    // UTILITIES
    int ConvertToRow(const int &idx) const;
    int ConvertToCol(const int &idx) const;
    int ConvertToIdx(const int &row, const int &col) const;

    private:
        // --- Deserialize Methods ---
        bool DeserializeGameInstance(const char* &filepath);
        bool DeserializeGameDef(json_t* game_instance);
        bool DeserializeGameState(json_t* game_instance);
        Array2D DeserializeArray2D(json_t* serialized_array2d, ElDeserializeFnPtr deserialize_function);
        void CreateGameboard();
        CandyPtr MakeCandy(int color, int type);
        long CalcMaxScore(Array2D score_board);

        // --- Serialize Methods ---
        bool SerializeGameInstance(const char* &filepath);
        Json_ptr SerializeGameDef(void);
        Json_ptr SerializeGameState(void);
        Json_ptr SerializeArray2D(Array2D array, json_t *(*serialize_fn)(Array2D));

        // --- Mutators ---
        void SetCandy(const int &idx, const int &color, const int &type);
        void SetCandy(const int &dest_idx, const int &source_idx);
        bool TrySwap(const int &idx1, const int &idx2);

        // --- Memory Management ---
        bool FreeCandy(const int &idx);
        void FreeModel();

        // ---Gameplay Methods ---
        bool HasVerticalMatch(const int &idx);
        bool HasHorizontalMatch(const int &idx);
        bool ScanSequence(const int size, vector<int> candy_seq);

        void SettleBoard();
        bool FireBoardLoop();
        bool FindAndFireTemplates(const int &num, const bool &isVertical);
        bool FireTemplate(int idx, const int &num, const int &increment);
        void ApplyGravity();
        void FillFromExtensionBoard();

        void AdjustScore(const int &idx); //if fires remaining, inc score and dec fires

        Array2D extension_board_; //extra candy to drop
        Array2D game_board_; //current visible board
        Array2D original_fired_state_;
        Array2D fired_state_; //how many times each cell needs to fire before win
        vector<int> extension_offset_; //row number for next drop position of extension board
        int sel_candy_idx_ = -1; //button-selected candy
        int game_id_;
        int total_moves_;
        int moves_made_ = 0;
        int num_colors_; //number of colors for this game board
        int max_score_;
        int score_ = 0;

        const int NO_CANDY = -1;
        const int MIN_MATCH_LENGTH = 3;
        const int DEFAULT_CANDY_TYPE = 0;
        const int MAX_SETTLE = 1000;
};

#endif // _GAME_MODEL_H_
