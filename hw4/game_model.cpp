#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <exception>
#include "game_model.h"

using namespace std;

extern "C" void DeserializeFunction(Array2D, Json_ptr);
extern "C" void DeserializeFunction2(Array2D, Json_ptr);

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

    for(int i = 0; i < game_board_->num_cols; i++) {
        extension_offset_.push_back(0);
    }

    game_id_ = 12345;
    total_moves_ = 20;
    moves_made_ = 0;
    num_colors_ = 6;
}

GameModel::GameModel(string filepath) {
    DeserializeGame(filepath);
}

int GameModel::GetMovesRemaining() const {
    return total_moves_ - moves_made_;
}

int GameModel::GetSelectedCandyIdx() const {
    return sel_candy_idx_;
}

CandyPtr GameModel::GetSelectedCandyPtr() const {
    return (CandyPtr) game_board_->data[sel_candy_idx_];
}

int GameModel::GetCandyColor(const int &idx) const {
    CandyPtr candy;
    candy = (CandyPtr) game_board_->data[idx];
    return candy->color;
}

int GameModel::GetRowLength() const {
    return game_board_->num_rows;
}

int GameModel::GetColLength() const {
    return game_board_->num_cols;
}

int GameModel::ConvertToRow(const int &idx) const {
    return idx / GetRowLength();
}

int GameModel::ConvertToCol(const int &idx) const {
    return idx % GetRowLength();
}

int GameModel::ConvertToIdx(const int &row, const int &col) const {
    return row * GetColLength() + col;
}

int GameModel::GetBoardSize() const {
    return game_board_->num_rows * game_board_->num_cols;
}

void GameModel::SetSelectedCandy(int idx) {
    sel_candy_idx_ = idx;
}

bool GameModel::SwapCandy(const char &dir) {
    int row2, col2, idx2;

    if(GetSelectedCandyPtr() == nullptr) {
        return false;
    }

    row2 = ConvertToRow(sel_candy_idx_);
    col2 = ConvertToCol(sel_candy_idx_);

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

    if(col2 < 0 || row2 < 0 || row2 >= GetRowLength() || col2 >= GetColLength()) {
        return false;
    }

    idx2 = ConvertToIdx(row2, col2);

    cout << "Preswap: " << endl;
    PrintBoard();

    if(TrySwap(sel_candy_idx_, idx2)) {

        moves_made_++;
        SetSelectedCandy(NO_CANDY);
        if(FireBoardLoop()) {
            cout << "Postswap: " << endl;
            PrintBoard();

            ApplyGravity();
            cout << "Post Gravity: " << endl;
            PrintBoard();
            return true;
        } else {
            //assert?
        }
    }

    return false;
}

bool GameModel::IsGameOver() {
    //TODO
    return false;
}

void GameModel::DeserializeGame(const string &filepath) {
    //TODO

}

void GameModel::SerializeGame(const string &filepath) {
    //TODO

}

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

// These could probably be optimized to work with any minimum match length,
// but works for now
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

// These could probably be optimized to work with any minimum match length,
// but works for now
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

void GameModel::SetCandy(const int &idx, const int &color, const int &type) {
    FreeCandy(idx);

    CandyPtr candy = (CandyPtr)malloc(sizeof(Candy));
    candy->color = color;
    candy->type = type;
    game_board_->data[idx] = (Array_t)(candy);
}

void GameModel::SetCandy(const int &dest_idx, const int &source_idx) {
    int new_color = GetCandyColor(source_idx);

    SetCandy(dest_idx, new_color, DEFAULT_CANDY_TYPE);
}

bool GameModel::FreeCandy(const int &idx) {
    Array_t old_candy = (Array_t) game_board_->data[idx];
    free(old_candy);
    return true;
}

bool GameModel::FireBoardLoop() {
    bool fired = FindAndFireVerticalMatch(4);
    fired |= FindAndFireHorizontalMatch(4);
    fired |= FindAndFireVerticalMatch(3);
    fired |= FindAndFireHorizontalMatch(3);

    return fired;
}

bool GameModel::FindAndFireVerticalMatch(const int &num) {
    bool fired = false;
    int idx;
    int seq_count = 0;
    int color = -2;

    for(int col = 0; col < GetRowLength(); col++) {
        for(int row = 0; row < GetColLength(); row++) {
            if(seq_count == num) {
                for(int i = 0; i < num; i++) {
                    SetCandy(idx, -1, 0);
                    AdjustScore(idx);
                    idx -= GetRowLength();
                }
                seq_count = 0;
                color = -2;
                fired = true;
            }

            idx = ConvertToIdx(row, col);

            if(GetCandyColor(idx) == color) {
                seq_count++;
            } else if(GetCandyColor(idx) == -1) {
                color = -2;
                seq_count = 0;
            } else {
                color = GetCandyColor(idx);
                seq_count = 1;
            }
        }
        if(seq_count == num) {
            for (int i = 0; i < num; i++) {
                SetCandy(idx, -1, 0);
                AdjustScore(idx);
                idx -= GetRowLength();
            }
        }
        seq_count = 0;
        color = -2;
        fired = true;
    }

    return fired;
}

bool GameModel::FindAndFireHorizontalMatch(const int &num) {
    int idx;
    int seq_count = 0;
    int color = -2;
    bool fired = false;

    for(int row = 0; row < GetColLength(); row++) {
        for(int col = 0; col < GetRowLength(); col++) {
            if(seq_count == num) {
                for(int i = 0; i < num; i++) {
                    SetCandy(idx, -1, 0);
                    AdjustScore(idx);
                    idx--;
                }
                seq_count = 0;
                color = -2;
                fired = true;
            }

            idx = ConvertToIdx(row, col);

            if(GetCandyColor(idx) == color) {
                seq_count++;
            } else if(GetCandyColor(idx) == -1) {
                color = -2;
                seq_count = 0;
            } else {
                color = GetCandyColor(idx);
                seq_count = 1;
            }
        }

        if(seq_count == num) {
            for (int i = 0; i < num; i++) {
                SetCandy(idx, -1, 0);
                AdjustScore(idx);
                idx--;
            }
        }
            seq_count = 0;
            color = -2;
            fired = true;
    }

    return fired;
}

void GameModel::AdjustScore(const int &idx) {
    long firings_left = (long)(fired_state_->data[idx]);

    if(firings_left > 0) {
        firings_left--;
        fired_state_->data[idx] = (Array_t)firings_left;
        score_++;
        cout << "firings left: " << long(fired_state_->data[idx]) << endl;
    } else {
        cout << "no firings left!" <<endl;
    }
}

void GameModel::ApplyGravity() {
    for(int dest_idx = 0; dest_idx < GetBoardSize() - GetRowLength(); dest_idx++) {
        if(GetCandyColor(dest_idx) == NO_CANDY) {
            int src_idx = dest_idx + GetRowLength();
            SetCandy(dest_idx, src_idx);
            SetCandy(src_idx, NO_CANDY, DEFAULT_CANDY_TYPE);
        }
    }
}

void GameModel::FillFromExtensionBoard() {
    //TODO

}

void GameModel::PrintBoard() {
    int idx = GetBoardSize() - 1;
    cout << "Current board state:" << endl;
    for(int i = GetRowLength(); i > 0; i--) {
        for(int j = GetColLength(); j > 0; j--) {
            cout << GetCandyColor(idx) << " ";
            idx--;
        }
        cout << endl;
    }
    cout << "============" << endl;
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

