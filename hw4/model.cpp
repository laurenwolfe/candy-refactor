#include "model.h"

Board::Board() {
    char *file1 = (char *)"test1.json";
    extension_board_ = AllocateArray2D();
    Deserialize(extension_board_, file1, DeserializeFunction);
//    extension_board_->num_cols = 2;
//    extension_board_->num_rows = 8;
//    extension_board_->size = 16;
//    extension_board_->data = (Array_t)malloc(sizeof(Array_t) * extension_board_->size);

    for(int i = 0; i < extension_board_->size; i++) {
        extension_board_->data[i] = (nullptr)((long)(i % 6));
    }
/*
    game_board_ = AllocateArray2D();
    game_board_->num_cols = 2;
    game_board_->num_rows = 2;
    game_board_->size = 4;
    game_board_->data = (Array_t)malloc(sizeof(Array_t) * game_board_->size);

    for(int i = 0; i < game_board_->size; i++) {
        int val = i % 3;
        game_board_->data[i].color = (nullptr)((long)val);
        game_board_->data[i].type = (int)(0);
    }

    fired_state_ = AllocateArray2D();
    fired_state_->num_cols = 2;
    fired_state_->num_rows = 2;
    fired_state_->size = 4;
    fired_state_->data = (Array_t)malloc(sizeof(Array_t) * fired_state_->size);

    for(int i = 0; i < fired_state_->size; i++) {
        fired_state_->data[i] = (int)(1);
    }

    for(int i = 0; i < game_board_->num_cols; i++) {
        extension_offset_ = vec.push_back(0);
    }
*/
    game_id_ = 12345;
    total_moves_ = 20;
    moves_left_ = 20;
    num_colors_ = 6;
}

//Deserialize data in json file provided as a command line argument
/*
void Board::DeserializeFunction(Array2D array, Json_ptr data) {
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
*/

Board::Board(string filepath) {
    DeserializeGame(filepath);
}

Candy Board::GetSelectedCandy() const {

}

int Board::GetRowLength() const {
    return -1;
}

int Board::GetColLength() const {
    return -1;
}

int Board::GetBoardSize() const {
    return -1;
}

bool Board::SwapCandy(const int &idx1, const int &idx2) {
    return false;
}

bool Board::IsGameOver() {
    return false;
}

void Board::DeserializeGame() {

}

void Board::SerializeGame() {

}

bool Board::IsValidSwap(const int &idx1, const int &idx2) {
    return false;
}

void Board::FireBoardLoop() {

}

void Board::FindMatches() {

}

void Board::FindVerticalMatches(const int &num) {

}

void Board::FindHorizontalMatches(const int &num) {

}

void Board::AdjustScore() {

}

void Board::ApplyGravity() {

}

void Board::FillFromExtensionBoard() {

}