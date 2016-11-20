#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <exception>
#include "game_model.h"

using namespace std;

extern "C" void DeserializeFunction(Array2D, Json_ptr);
extern "C" void DeserializeFunction2(Array2D, Json_ptr);

// Empty Constructor is for testing purposes
GameModel::GameModel() {
    char *file1 = (char *)"test1.json";
    char *file2 = (char *)"test2.json";
    char *file3 = (char *)"test3.json";

    extension_board_ = AllocateArray2D();
    Deserialize(extension_board_, file1, &DeserializeFunction);

    game_board_ = AllocateArray2D();
    Deserialize(game_board_, file2, &DeserializeFunction2);

    fired_state_ = AllocateArray2D();
    Deserialize(fired_state_, file3, &DeserializeFunction);

    original_fired_state_ = AllocateArray2D();
    Deserialize(original_fired_state_, file3, &DeserializeFunction);

    for(int i = 0; i < game_board_->num_cols; i++) {
        extension_offset_.push_back(0);
    }

    game_id_ = 12345;
    total_moves_ = 20;
    moves_made_ = 0;
    num_colors_ = 6;
    max_score_ = GetMaxScore();
}

// Constructs a GameModel instance by deserializing game data file
GameModel::GameModel(string filepath) {
    DeserializeGame(filepath);
}

// Returns number of moves remaining
int GameModel::GetMovesRemaining() const {
    return total_moves_ - moves_made_;
}

// Returns max possible score (total number of possible firings for board)
int GameModel::GetMaxScore() const {
    int max_score = 0;

    for(int i = 0; i < GetBoardSize(); i++) {
        max_score += (long)original_fired_state_->data[i];
    }

    return max_score;
}

// Returns the index of the candy currently selected on the gameboard
int GameModel::GetSelectedCandyIdx() const {
    return sel_candy_idx_;
}

// Returns int representing candy's color
int GameModel::GetCandyColor(const int &idx) const {
    CandyPtr candy;
    candy = (CandyPtr) game_board_->data[idx];
    return candy->color;
}

// Returns the total number of board squares
int GameModel::GetBoardSize() const {
    return game_board_->num_rows * game_board_->num_cols;
}

// Returns length of board row (horizontal)
int GameModel::GetRowLength() const {
    return game_board_->num_rows;
}

// Returns length of board column (vertical)
int GameModel::GetColLength() const {
    return game_board_->num_cols;
}

// Given an index, convert to row position
int GameModel::ConvertToRow(const int &idx) const {
    return idx / GetRowLength();
}

// Given an index, convert to column position
int GameModel::ConvertToCol(const int &idx) const {
    return idx % GetRowLength();
}

// Given a row and column, convert to array index
int GameModel::ConvertToIdx(const int &row, const int &col) const {
    return row * GetColLength() + col;
}

// Prints a copy of the current board's candy color values
// in a grid alignment. Blank spots indicate fired squares
// that haven't yet been filled.
void GameModel::PrintBoard() {
    int idx = GetBoardSize() - 1;
    cout << "Current board state:" << endl;
    for(int i = GetRowLength(); i > 0; i--) {
        for(int j = GetColLength(); j > 0; j--) {
            int color = GetCandyColor(idx);
            if(color == -1) {
                cout << "  ";
            } else {
                cout << color << " ";
            }
            idx--;
        }
        cout << endl;
    }
    cout << "============" << endl;
}

// Returns true if all squares have been fully fired or
// if no moves remain, false otherwise
bool GameModel::IsGameOver() {
    if(score_ == max_score_) {
        return true;
    } else if(GetMovesRemaining() == 0) {
        return true;
    } else {
        return false;
    }
}

// Set index of candy selected on gameboard
// Resets to NO_CANDY after a swap is executed
void GameModel::SetSelectedCandy(int idx) {
    sel_candy_idx_ = idx;
}

bool GameModel::SwapCandy(const char &dir) {
    int row2, col2, idx2, settle_ctr;

    if(GetSelectedCandyIdx() == NO_CANDY || IsGameOver()) {
        return false;
    }

    settle_ctr = 0;
    row2 = ConvertToRow(sel_candy_idx_);
    col2 = ConvertToCol(sel_candy_idx_);

    // alter row/column values for square to swap into based on move direction
    if(dir == 'N') {
        row2++;
    } else if(dir == 'S') {
        row2--;
    } else if(dir == 'E') {
        col2--;
    } else if(dir == 'W') {
        col2++;
    } else {
        return false;
    }

    // if the more direction is invalid (goes off the board), return false
    if(col2 < 0 || row2 < 0 || row2 >= GetRowLength() || col2 >= GetColLength()) {
        return false;
    }

    idx2 = ConvertToIdx(row2, col2);

    cout << "Preswap: " << endl;
    PrintBoard();

    // Swaps the candies and verifies that a match is made by doing so.
    // If invalid swap, re-swaps the candies and returns false.
    if(TrySwap(sel_candy_idx_, idx2)) {
        moves_made_++;
        // Loop fires templates in order of priority,
        // applies gravity, fills empty slots, and checks for
        // new matches. Executes until board settles or loop has executed
        // a maximum number of times.
        while(settle_ctr < MAX_SETTLE && FireBoardLoop()) {
            cout << "Post Fire: " << endl;
            PrintBoard();

            ApplyGravity();
            cout << "Post Gravity: " << endl;
            PrintBoard();

            FillFromExtensionBoard();
            cout << "Post Fill: " << endl;
            PrintBoard();

            settle_ctr++;
        }
        // Reset selected candy
        SetSelectedCandy(NO_CANDY);
        return true;
    }
    return false;
}

void GameModel::DeserializeGame(const string &filepath) {
    //TODO

}

void GameModel::SerializeGame(const string &filepath) {
    //TODO

}

// Frees old candy, creates a new candy, adds to gameboard array
void GameModel::SetCandy(const int &idx, const int &color, const int &type) {
    FreeCandy(idx);

    CandyPtr candy = (CandyPtr)malloc(sizeof(Candy));
    candy->color = color;
    candy->type = type;
    game_board_->data[idx] = (Array_t)(candy);
}

// Takes candy data from source index and fills candy at dest index
void GameModel::SetCandy(const int &dest_idx, const int &source_idx) {
    int new_color = GetCandyColor(source_idx);

    SetCandy(dest_idx, new_color, DEFAULT_CANDY_TYPE);
}

// Free Candy struct
bool GameModel::FreeCandy(const int &idx) {
    Array_t old_candy = (Array_t) game_board_->data[idx];
    free(old_candy);
    return true;
}

// Swaps two candies and looks to see if swapping creates at
// least one template, confirming that swap is valid.
// Returns true if valid, false otherwise
bool GameModel::TrySwap(const int &idx1, const int &idx2) {
    bool matched;

    Swap(game_board_, idx1, idx2);

    matched = HasVerticalMatch(idx1) || HasVerticalMatch(idx2) ||
         HasHorizontalMatch(idx1) || HasHorizontalMatch(idx2);
    if(!matched) {
        Swap(game_board_, idx1, idx2);
    }
    return matched;
}

// Looks for a match along the vertical interval of a given index.
// Returns true if found, false otherwise.
bool GameModel::HasVerticalMatch(const int &idx) {
    int size = 0;
    vector<int> candy_seq;
    //min/max difference for index
    const int interval = GetRowLength() * (MIN_MATCH_LENGTH - 1);

    if(GetColLength() >= MIN_MATCH_LENGTH) {
        //for every valid candy index within interval,
        //add candy color to vector
        for (int i = idx - interval; i <= (idx + interval); i = i + GetRowLength()) {
            if (i > 0 && i < GetBoardSize()) {
                candy_seq.push_back(GetCandyColor(i));
                size++;
            }
        }

        return ScanSequence(size, candy_seq);
    }
    return false;
}

// Looks for a match along the horizontal interval of a given index.
// Returns true if found, false otherwise.
bool GameModel::HasHorizontalMatch(const int &idx) {
    int size, max_row_idx, max_match_idx, max_idx;
    size = 0;
    vector<int> candy_seq;

    if(GetRowLength() >= MIN_MATCH_LENGTH) {
        // Get maximum index (+1) possible for matching
        // by taking minimum of max row index and max match index
        max_row_idx = idx + GetRowLength() - idx % GetRowLength();
        max_match_idx = idx + MIN_MATCH_LENGTH;
        max_idx = min(max_match_idx, max_row_idx);

        //for every valid candy index above and below given index,
        //add candy color to vector
        for (int i = idx - (MIN_MATCH_LENGTH - 1);
                 i < idx + (MIN_MATCH_LENGTH - 1); i++) {
            if (i > 0 && i < max_idx) {
                candy_seq.push_back(GetCandyColor(i));
                size++;
            }
        }

        return ScanSequence(size, candy_seq);
    }
    return false;
}

// Scans candy sequence vector looking for a template of at least minimum length
bool GameModel::ScanSequence(const int size, vector<int> candy_seq) {
    int seq_count = 0;
    int color = -2;

    for(int i = 0; i < size; i++) {
        //Found sequence!
        if(seq_count == MIN_MATCH_LENGTH) {
            return true;
        }

        //If current candy continues sequence, increment, otherwise reset
        if(candy_seq[i] == color) {
            seq_count++;
        } else {
            color = candy_seq[i];
            seq_count = 1;
        }
    }
    //check for last element satisfy minimum match length
    if(seq_count == MIN_MATCH_LENGTH) {
        return true;
    }
    return false;
}

// Fire candy templates in order of priority: vFour, hFour, vThree, hThree.
// Returns true if any firings occur.
bool GameModel::FireBoardLoop() {
    bool fired = FindAndFireTemplates(4, true);
    fired |= FindAndFireTemplates(4, false);
    fired |= FindAndFireTemplates(3, true);
    fired |= FindAndFireTemplates(3, false);

    return fired;
}

// Find templates, starting with index 0.
bool GameModel::FindAndFireTemplates(const int &num, const bool &isVertical) {
    bool fired;
    int seq_count, color, length, width, increment, idx;

    // Set the direction and increment interval for each index,
    // allowing the function to be used for vertical or horizontal moves
    if(isVertical) {
        length = GetColLength();
        width = GetRowLength();
        increment = GetRowLength();
    } else {
        length = GetRowLength();
        width = GetColLength();
        increment = 1;
    }

    idx = 0;
    fired = false;

    for(int i = 0; i < length; i++) {
        seq_count = 0;
        color = -2;
        for(int j = 0; j < width; j++) {
            if(GetCandyColor(idx) == NO_CANDY) {
                color = -2;
                seq_count = 0;
            } else if(GetCandyColor(idx) == color) {
                seq_count++;

                if(seq_count == num) {
                    fired = FireTemplate(idx, num, increment);
                    color = -2;
                    seq_count = 0;
                }
            } else {
                color = GetCandyColor(idx);
                seq_count = 1;
            }
            idx = (idx + increment) % (GetBoardSize() - 1);
        }
        if(seq_count == num) {
            fired = FireTemplate(idx, num, increment);
        }
    }

    return fired;
}

// If template is found, fire those candies, marking those candy's
// color values as fired
bool GameModel::FireTemplate(int idx, const int &num, const int &increment) {

    for(int seq_len = 0; seq_len < num; seq_len++) {
        SetCandy(idx, NO_CANDY, DEFAULT_CANDY_TYPE);
        AdjustScore(idx);
        idx -= increment;
    }

    return true;
}

// Adjust score and decrement firings remaining for square, if any.
void GameModel::AdjustScore(const int &idx) {
    long firings_left = (long)(fired_state_->data[idx]);

    if(firings_left > 0) {
        firings_left--;
        fired_state_->data[idx] = (Array_t)firings_left;
        score_++;
    }
}

// Drop candies from higher indices to fill empty squares (vertically)
void GameModel::ApplyGravity() {
    int src_idx;
    for(int dest_idx = 0; dest_idx < GetBoardSize() - GetRowLength(); dest_idx++) {
        src_idx = dest_idx + GetRowLength();
        while (GetCandyColor(dest_idx) == NO_CANDY && src_idx < GetBoardSize()) {
            if(GetCandyColor(src_idx) != NO_CANDY) {
                SetCandy(dest_idx, src_idx);
                SetCandy(src_idx, NO_CANDY, DEFAULT_CANDY_TYPE);
            } else {
                src_idx += GetRowLength();
            }
        }
    }
}

// Fill empty squares with candies from extension board, based on next
// row indicated in extension offset vector (representing each column)
void GameModel::FillFromExtensionBoard() {
    cout << "Attempting to Fill" << endl;
    for(int idx = 0; idx < GetBoardSize(); idx++) {
        if(GetCandyColor(idx) == NO_CANDY) {
            int extension_col = ConvertToCol(idx);
            int extension_row =
                extension_offset_.at(extension_col) % extension_board_->num_rows;
            int extension_idx = ConvertToIdx(extension_row, extension_col);
            int new_color = (long)extension_board_->data[extension_idx];

            SetCandy(idx, new_color, DEFAULT_CANDY_TYPE);
            extension_offset_.at(extension_col) += 1;
        }
    }
}

//Deserialize data in json file provided as a command line argument
void DeserializeFunction(Array2D array, Json_ptr data) {
    Json_ptr value;
    size_t index;
    int el_value;

    //Unpack array of integers from JSON object into the Array2D
    json_array_foreach(data, index, value) {
        json_unpack(value, "i", &el_value);
        array->data[index] = (Json_ptr)((long)el_value);
    }
    json_array_clear(data);
}

//Deserialize data in json file provided as a command line argument
void DeserializeFunction2(Array2D array, Json_ptr data) {
    Json_ptr value;
    size_t index;
    int el_value;

    //Unpack array of integers from JSON object into the Array2D
    json_array_foreach(data, index, value) {
        CandyPtr candy = (CandyPtr)malloc(sizeof(Candy));
        json_unpack(value, "i", &el_value);
        candy->color = el_value;
        candy->type = 0;
        array->data[index] = (Json_ptr)(candy);
    }
    json_array_clear(data);
}

