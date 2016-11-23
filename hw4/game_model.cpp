#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <exception>
#include "game_model.h"

using namespace std;

extern "C" void DeserializeIntFunction(Array2D, Json_ptr);
extern "C" void DeserializeCandyFunction(Array2D, Json_ptr);
extern "C" json_t* SerializeCandyFunction(Array2D array);
extern "C" json_t* SerializeIntFunction(Array2D array);
extern "C" void FreeCandyFunction(int idx);
extern "C" void FreeIntFunction(int idx);

/////////////////////////////////////////////////////////////
/////////// PUBLIC METHODS
/////////////////////////////////////////////////////////////

// Constructs a GameModel instance by deserializing game data file
GameModel::GameModel(const string &filepath) {
    const char* c_filepath = filepath.c_str();
    DeserializeGameInstance(c_filepath);
}

/////////////////////////////////////////////////////////////
// Getters
/////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////
// Other Public Methods
/////////////////////////////////////////////////////////////

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

void GameModel::PrintBoard(const Array2D &board) {
    int idx = board->size - 1;
    for(int i = board->num_rows; i > 0; i--) {
        for(int j = board->num_cols; j > 0; j--) {
            int color = (long)board->data[idx];
            cout << color << " ";
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
    int row2, col2, idx2;

    if(GetSelectedCandyIdx() == NO_CANDY || IsGameOver()) {
        return false;
    }

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

        SettleBoard();
        // Reset selected candy
        SetSelectedCandy(NO_CANDY);
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////
/////////// PRIVATE METHODS
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Deserialize Methods
/////////////////////////////////////////////////////////////

bool GameModel::DeserializeGameInstance(const char* &filepath){
    Json_ptr json_gameinstance;
    json_error_t error;

    json_gameinstance = json_load_file(filepath, 0, &error); // get json object
    if (!json_gameinstance) { // error reading json file
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return false;
    }

    //Deserialize gamedef
    if(!DeserializeGameDef(json_gameinstance))
        return false;

    if (!DeserializeGameState(json_gameinstance)) { // Gamestate was not present
        game_board_ = AllocateArray2D();
        if(game_board_ == nullptr)
            return false;
        CreateGameboard();

        // allocate mem for fired_state, copy contents of
        // original_fired_state->data into it
        fired_state_ = AllocateArray2D();
        if(fired_state_ == nullptr)
            return false;
        *(fired_state_) = *(original_fired_state_);
        fired_state_->data = (Array_t *)calloc(fired_state_->size, sizeof(Array_t));

        if(fired_state_->data == nullptr) {
            return false;
        }

        size_t num_bytes_of_data = (fired_state_->size * sizeof(Array_t));
        memcpy(fired_state_->data, original_fired_state_->data, num_bytes_of_data);
    }

    json_decref(json_gameinstance);

    SettleBoard();

    return true;
}

bool GameModel::DeserializeGameDef(json_t* &game_instance){
    Json_ptr json_gamedef, json_extensioncolor, json_boardstate;
    int gameid, movesallowed, colors;

    Array2D extensioncolor, boardstate;

    json_gamedef = json_object_get(game_instance, "gamedef");
    if (json_gamedef == nullptr) {
        fprintf(stderr, "could not find 'gamedef' in json file");
        return false;
    }

    json_unpack(json_gamedef, "{s:i, s:o, s:o, s:i, s:i}", "gameid", &gameid, "extensioncolor", &json_extensioncolor, "boardstate", &json_boardstate, "movesallowed", &movesallowed, "colors", &colors);

    extensioncolor = DeserializeArray2D(json_extensioncolor, DeserializeIntFunction);
    boardstate = DeserializeArray2D(json_boardstate, DeserializeIntFunction);
    if(extensioncolor == nullptr){ return false; } // out of memory
    if(boardstate == nullptr){ return false; } // out of memory

    // assign values obtained from json file to this game object
    game_id_ = gameid;
    extension_board_ = extensioncolor;
    original_fired_state_ = boardstate;
    total_moves_ = movesallowed;
    num_colors_ = colors;
    max_score_ = CalcMaxScore(boardstate);

    return true;
}

bool GameModel::DeserializeGameState(json_t* &game_instance) {
    json_t* json_gamestate;
    int movesmade, currentscore;
    json_t *json_boardcandies, *json_boardstate, *json_extensionoffset;
    Array2D boardcandies, boardstate;

    json_gamestate = json_object_get(game_instance, "gamestate");
    if (json_gamestate == nullptr) { // gamestate wasn't found
        return false;
    }

    json_unpack(json_gamestate, "{s:o, s:o, s:i, s:i, s:o}", "boardcandies", &json_boardcandies, "boardstate", &json_boardstate, "movesmade", &movesmade, "currentscore", &currentscore, "extensionoffset", &json_extensionoffset);

    boardstate = DeserializeArray2D(json_boardstate, DeserializeIntFunction);
    boardcandies = DeserializeArray2D(json_boardcandies, DeserializeCandyFunction);
    if(boardstate == nullptr || boardcandies == nullptr){
        return false;
    } //out of memory

    // create the extension offset
    size_t idx;
    json_t* value;
    int jint;
    vector<int> extensionoffset (GetNumCols(original_fired_state_), 0);
    json_array_foreach(json_extensionoffset, idx, value) {
        // add int val from  json_extension_offset
        json_unpack(value, "i", &jint);
        extensionoffset[idx] = jint;
    }

    // assign values obtained from json file to this game object
    game_board_ = boardcandies;
    fired_state_ = boardstate;
    moves_made_ = movesmade;
    score_ = currentscore;
    extension_offset_ = extensionoffset;

    return true;
}

Array2D GameModel::DeserializeArray2D(json_t* &serialized_array2d, ElDeserializeFnPtr deserialize_function){
    Array2D array_field;
    json_t* data;
    int rows, cols;
    ElDeserializeFnPtr deserialize_fn;

    deserialize_fn = deserialize_function;

    // allocate space for array2D on heap
    array_field = AllocateArray2D();
    if (array_field == NULL) { return (Array2D)NULL; } // out of memory

    json_unpack(serialized_array2d, "{s:i, s:i, s:o}", "rows", &rows,
                "columns", &cols, "data", &data);

    //insert deserialized values into Array2D
    array_field->num_rows = rows;
    array_field->num_cols = cols;
    array_field->size = (cols * rows);

    array_field->data = (Array_t*)malloc(sizeof(Array_t) * (array_field->size));
    if (array_field == NULL) { return (Array2D)NULL; } // out of memory

    // Use client-provided function to insert data into array
    deserialize_fn(array_field, data);
    return array_field;
}

bool GameModel::CreateGameboard() {
    game_board_->num_rows = original_fired_state_->num_rows;
    game_board_->num_cols = original_fired_state_->num_cols;
    game_board_->size = game_board_->num_rows * game_board_->num_cols;

    game_board_->data = (Array_t*)malloc(sizeof(Array_t) * (game_board_->size));

    if(game_board_->data == nullptr) {
        return false;
    }

    for(int i = 0; i < game_board_->size; i++) {
        CandyPtr candy_ptr = MakeCandy((long)extension_board_->data[i], DEFAULT_CANDY_TYPE);
        game_board_->data[i] = (Array_t)candy_ptr;
    }

    for(int i = 0; i < game_board_->num_cols; i++) {
        extension_offset_.push_back(game_board_->num_rows);
    }

    return true;
}

CandyPtr GameModel::MakeCandy(const int &color, const int &type) {
    CandyPtr candy_ptr = (CandyPtr)malloc(sizeof(Candy));
    if(candy_ptr == nullptr) {
        return (CandyPtr)NULL;
    }
    candy_ptr->color = color;
    candy_ptr->type = type;
    return candy_ptr;
}

// subroutine used to calculate the max score before the came has been initialized
long GameModel::CalcMaxScore(Array2D score_board){
    long max_score = 0;
    for (int i = 0; i < GetSize(score_board); i++) {
        max_score += (long)GetElement(score_board, i);
    }
    return max_score;
}

void DeserializeIntFunction(Array2D array, Json_ptr data) {
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

void DeserializeCandyFunction(Array2D array, Json_ptr data) {
    Json_ptr value;
    size_t index;
    int jcolor;
    int jtype;

    //Unpack array of Candies from JSON object into the Array2D
    json_array_foreach(data, index, value) {
        json_unpack(value, "{s:i, s:i}", "color", &jcolor, "type", &jtype);
        MakeCandy(jcolor, jtype);

        array->data[index] = (Json_ptr)(candy);
    }
    json_array_clear(data);
}

/////////////////////////////////////////////////////////////
// Serialize Methods
/////////////////////////////////////////////////////////////

void GameModel::SerializeGame(const string &filepath) {
    const char* c_filepath = filepath.c_str();
    bool result;
    result = SerializeGameInstance(c_filepath);
    if(!result){ // something failed
        cout << "Encountered an error while trying to serialize game instance." << endl;
    }
}

bool GameModel::SerializeGameInstance(const char* &filepath) {
    Json_ptr json_gamedef, json_gamestate, json_str;

    json_gamedef = SerializeGameDef();
    json_gamestate = SerializeGameState();

    json_str = json_pack("{s:o, s:o}", "gamedef", json_gamedef, "gamestate", json_gamestate);

    json_dump_file(json_str, filepath, JSON_INDENT(10));

    json_decref(json_str);

    return true;
}

Json_ptr GameModel::SerializeGameDef(void){
    Json_ptr gamedef, json_extensioncolor, json_boardstate;

    json_extensioncolor = SerializeArray2D(extension_board_, SerializeIntFunction);
    json_boardstate = SerializeArray2D(original_fired_state_, SerializeIntFunction);

    gamedef = json_pack("{s:i, s:o, s:o, s:i, s:i}",
                        "gameid", game_id_,
                        "extensioncolor", json_extensioncolor,
                        "boardstate", json_boardstate,
                        "movesallowed", total_moves_,
                        "colors", num_colors_);

    return gamedef;
}

Json_ptr GameModel::SerializeGameState(void){
    Json_ptr gamestate, json_boardcandies, json_boardstate, json_extensionoffset;

    json_boardcandies = SerializeArray2D(game_board_, SerializeCandyFunction);
    json_boardstate = SerializeArray2D(fired_state_, SerializeIntFunction);
    json_extensionoffset = json_array();

    for(auto const &value:extension_offset_) {
        json_array_append_new(json_extensionoffset, json_integer(value));
    }

    gamestate = json_pack("{s:o, s:o, s:i, s:i, s:o}",
                          "boardcandies", json_boardcandies,
                          "boardstate", json_boardstate,
                          "movesmade", moves_made_,
                          "currentscore", score_,
                          "extensionoffset", json_extensionoffset);

    return gamestate;
}

Json_ptr GameModel::SerializeArray2D(Array2D array, Json_ptr (*serialize_fn)(Array2D)) {
    Json_ptr json_data, json_arr;

    json_data = serialize_fn(array);

    json_arr = json_pack("{s:i, s:i, s:o}",
                         "rows", array->num_rows,
                         "columns", array->num_cols,
                         "data", json_data);

    return json_arr;
}

Json_ptr SerializeCandyFunction(Array2D array){
    Json_ptr json_arr = json_array();

    for (int i = 0; i < array->size; i++) {
        CandyPtr candy = (CandyPtr)array->data[i];
        Json_ptr json_candy = json_pack("{s:i s:i}", "color", candy->color, "type", candy->type);
        json_array_append_new(json_arr, json_candy);
    }
    return json_arr;
}

Json_ptr SerializeIntFunction(Array2D array) {
    Json_ptr json_arr = json_array();


    for (int i = 0; i < array->size; i++) {
        long val = (long)array->data[i];
        json_array_append_new(json_arr, json_integer(val));
    }

    return json_arr;
}

/////////////////////////////////////////////////////////////
// Mutators
/////////////////////////////////////////////////////////////

// Frees old candy, creates a new candy, adds to gameboard array
void GameModel::SetCandy(const int &idx, const int &color, const int &type) {
    FreeCandy(idx);

    CandyPtr candy = MakeCandy(color, type);

    game_board_->data[idx] = (Array_t)(candy);
}

// Takes candy data from source index and fills candy at dest index
void GameModel::SetCandy(const int &dest_idx, const int &source_idx) {
    int new_color = GetCandyColor(source_idx);

    SetCandy(dest_idx, new_color, DEFAULT_CANDY_TYPE);
}


// Swaps two candies and looks to see if swapping creates at
// least one template, confirming that swap is valid.
// Returns true if valid, false otherwise
bool GameModel::TrySwap(const int &idx1, const int &idx2) {
    bool matched;

    cout << "Swap " << idx1 << " and " << idx2 << endl;

    Swap(game_board_, idx1, idx2);

    matched = HasVerticalMatch(idx1) || HasVerticalMatch(idx2) ||
              HasHorizontalMatch(idx1) || HasHorizontalMatch(idx2);
    if(!matched) {
        Swap(game_board_, idx1, idx2);
    }
    return matched;
}

/////////////////////////////////////////////////////////////
// Memory Management Methods
/////////////////////////////////////////////////////////////


// Free Candy struct
bool GameModel::FreeCandy(const int &idx) {
    Array_t old_candy = (Array_t) game_board_->data[idx];
    free(old_candy);

    return true;
}

void FreeCandyFunction(Array_t candy_ptr) {
    free(candy_ptr);
}

void FreeIntFunction(Array_t ptr) {
    return;
}

void GameModel::FreeArrays() {
    for(int i = 0; i < GetBoardSize(); i++) {
        FreeCandy(i);
    }

    DestroyArray2D(extension_board_, &FreeIntFunction);
    DestroyArray2D(original_fired_state_, &FreeIntFunction);
    DestroyArray2D(fired_state_, &FreeIntFunction);
    DestroyArray2D(game_board_, &FreeCandyFunction);
}

/////////////////////////////////////////////////////////////
// Gameplay Methods
/////////////////////////////////////////////////////////////


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
        cout << "V Indexes to check: ";
        for (int i = idx - interval; i <= (idx + interval); i = i + GetRowLength()) {
            cout << i << " ";
            if (i >= 0 && i < GetBoardSize()) {
                candy_seq.push_back(GetCandyColor(i));
                size++;
            }
        }

        cout << endl;

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
        cout << "H Indexes to check: ";
        for (int i = idx - (MIN_MATCH_LENGTH - 1);
                 i < idx + MIN_MATCH_LENGTH; i++) {
            cout << i << " ";
            if (i >= 0 && i < max_idx) {
                candy_seq.push_back(GetCandyColor(i));
                size++;
            }
        }
        cout << endl;
        return ScanSequence(size, candy_seq);
    }
    return false;
}

// Scans candy sequence vector looking for a template of at least minimum length
bool GameModel::ScanSequence(const int size, vector<int> candy_seq) {
    int seq_count = 0;
    int color = -2;

    cout << "Check Sequence: ";
    for(int i = 0; i < size; i++) {
        cout << candy_seq[i] << " ";
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
    cout << endl;
    //check for last element satisfy minimum match length
    if(seq_count == MIN_MATCH_LENGTH) {
        return true;
    }
    return false;
}

// Loop fires templates in order of priority,
// applies gravity, fills empty slots, and checks for
// new matches. Executes until board settles or loop has executed
// a maximum number of times.
void GameModel::SettleBoard() {
    int settle_ctr = 0;

    while(settle_ctr < MAX_SETTLE && FireBoardLoop()) {
        ApplyGravity();
        cout << "Post Gravity: " << endl;
        PrintBoard();

        FillFromExtensionBoard();
        cout << "Post Fill: " << endl;
        PrintBoard();

        settle_ctr++;
    }
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
            if(idx + increment >= GetBoardSize()) {
                idx = (idx + increment + 1) % (GetBoardSize());
            } else {
                idx = idx + increment;
            }
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
        firings_left = firings_left - 1;
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

/////////////////////////////////////////////////////////////
// Utilities
/////////////////////////////////////////////////////////////

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