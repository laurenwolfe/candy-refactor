#include <iostream>
#include <vector>
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
    moves_left_ = 20;
    num_colors_ = 6;
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
        struct Candy *candy = (Candy *)malloc(sizeof(candy));
        json_unpack(value, "i", &el_value);
        candy->color = el_value;
        candy->type = 0;
        array->data[index] = (Json_ptr)(candy);
    }
    json_array_clear(data);
}


GameModel::GameModel(string filepath) {
    DeserializeGame(filepath);
}

Candy GameModel::GetSelectedCandy() const {
    Candy c;
    return c;
}

int GameModel::GetRowLength() const {
    return game_board_->num_rows;
}

int GameModel::GetColLength() const {
    return game_board_->num_cols;
}

int GameModel::GetBoardSize() const {
    return game_board_->num_rows * game_board_->num_cols;
}

bool GameModel::SwapCandy(const int &idx1, const int &idx2) {
    return false;
}

bool GameModel::IsGameOver() {
    return false;
}

void GameModel::DeserializeGame(const string &filepath) {

}

void GameModel::SerializeGame(const string &filepath) {

}

bool GameModel::IsValidSwap(const int &idx1, const int &idx2) {
    return false;
}

void GameModel::FireBoardLoop() {

}

void GameModel::FindMatches() {

}

void GameModel::FindVerticalMatches(const int &num) {

}

void GameModel::FindHorizontalMatches(const int &num) {

}

void GameModel::AdjustScore() {

}

void GameModel::ApplyGravity() {

}

void GameModel::FillFromExtensionBoard() {

}