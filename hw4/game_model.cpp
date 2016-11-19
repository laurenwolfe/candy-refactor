#include <iostream>
#include <vector>
#include <array>
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
    int row1, row2, col1, col2, idx1, idx2;

    if(GetSelectedCandyPtr() == nullptr) {
        return false;
    }

    row1 = row2 = ConvertToRow(sel_candy_idx_);
    col1 = col2 = ConvertToCol(sel_candy_idx_);

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

    idx1 = sel_candy_idx_;
    idx2 = ConvertToIdx(row2, col2);

    if(TrySwap(idx1, idx2)) {
        moves_made_++;
        SetSelectedCandy(NO_CANDY);
        FireBoardLoop();
        return true;
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
    int size, arr_idx, seq_count, color;
    size = GetColLength();
    arr_idx = 0, seq_count = 0, color = -2;
    int candy_seq[size];

    if(GetRowLength() >= MIN_MATCH_LENGTH) {
        //for every valid candy index above and below given index,
        //add candy color to vector
        for(int i = 0; i < idx + (GetRowLength() * MIN_MATCH_LENGTH);
            i + GetRowLength()) {
            if(i > 0 && i < GetBoardSize()) {
                candy_seq[arr_idx] = GetCandyColor(i);
                arr_idx++;
            }
        }

        for(int i = 0; i < arr_idx; i++) {
            //Found sequence!
            if(seq_count == MIN_MATCH_LENGTH) {
                return true;
            }

            if(candy_seq[i] == color) {
                //matches sequence
                seq_count++;
            } else {
                //discontinuous
                color = candy_seq[i];
                seq_count = 1;
            }
        }
        //check for last element satisfy minimum match length
        if(seq_count == MIN_MATCH_LENGTH) {
            return true;
        }
    }
    return false;
}

// These could probably be optimized to work with any minimum match length,
// but works for now
bool GameModel::HasHorizontalMatch(const int &idx) {
    int col = ConvertToCol(idx);

    if(GetColLength() >= 3) {
        if(col >= 2 &&
            (GetCandyColor(idx - 2)
            == GetCandyColor(idx - 1)
            == GetCandyColor(idx))) {
            // 2 left + idx + 0 right
            return true;
        } else if(col >= 1 && col + 1 < GetColLength() &&
            (GetCandyColor(idx - 1)
            == GetCandyColor(idx)
            == GetCandyColor(idx + 1))) {
            // 1 left + idx + 1 right
            return true;
        } else if(col + 2 < GetColLength() &&
            (GetCandyColor(idx)
            == GetCandyColor(idx + 1)
            == GetCandyColor(idx + 2))) {
            // 0 left + idx + 2 right
            return true;
        }
    }
    return false;
}

void GameModel::FireBoardLoop() {
    //TODO

}

void GameModel::FindMatches() {
    //TODO

}

void GameModel::FindVerticalMatches(const int &num) {
    //TODO

}

void GameModel::FindHorizontalMatches(const int &num) {
    //TODO

}

void GameModel::AdjustScore() {
    //TODO

}

void GameModel::ApplyGravity() {
    //TODO

}

void GameModel::FillFromExtensionBoard() {
    //TODO

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
