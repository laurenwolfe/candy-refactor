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

// CONSTRUCTORS ===============================================================
GameModel::GameModel() {
    char *file1 = (char *)"test1.json";
    char *file2 = (char *)"test2.json";
    char *file3 = (char *)"test3.json";

    extension_board_ = AllocateArray2D();
    Deserialize(extension_board_, file1, &DeserializeIntFunction);

    game_board_ = AllocateArray2D();
    Deserialize(game_board_, file2, &DeserializeCandyFunction);

    fired_state_ = AllocateArray2D();
    Deserialize(fired_state_, file3, &DeserializeIntFunction);

    for(int i = 0; i < game_board_->num_cols; i++) {
        extension_offset_.push_back(0);
    }

    game_id_ = 12345;
    total_moves_ = 20;
    moves_made_ = 0;
    num_colors_ = 6;
}

GameModel::GameModel(const string &filepath) {
    const char* c_filepath = filepath.c_str();
    if(DeserializeGameInstance(c_filepath)){
      // Deserialization was successsful
    } else {
      // an error occured
      //TODO: something?
    }
}
// =========================================================== END CONSTRUCTORS

// PUBLIC METHODS =============================================================
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
    int row2, col2, idx2, settle_ctr;

    settle_ctr = 0;

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
        while(settle_ctr < MAX_SETTLE && FireBoardLoop()) {
            moves_made_++;

            cout << "Post Fire: " << endl;
            PrintBoard();

            ApplyGravity();
            cout << "Post Gravity: " << endl;
            PrintBoard();

            FillFromExtensionBoard();

            cout << "Post Fill: " << endl;
            PrintBoard();

            /*
            cout << "Extension offsets: ";
            for(auto it = extension_offset_.begin();
                it < extension_offset_.end(); it++) {
                cout << *it << " ";
            }
            cout << endl;
            */
            settle_ctr++;
        }

        SetSelectedCandy(NO_CANDY);
        return true;
    }

    return false;
}

bool GameModel::IsGameOver() {
    //TODO
    return false;
}
// ========================================================= END PUBLIC METHODS

// PRIVATE METHODS ============================================================

bool GameModel::DeserializeGameInstance(const char* &filepath){
  json_t *json_gameinstance, *json_gamedef, *json_gamestate;
  json_error_t error;
  bool return_success;
  int return_int;

  json_gameinstance = json_load_file(filepath, 0, &error); // get json object
  if (!json_gameinstance) { // error reading json file
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return false;
  }

  return_success = DeserializeGameDef(json_gameinstance);
  if(!return_success){ return false; } // some error occured 
  
  return_int = DeserializeGameState(json_gameinstance);
  if (return_int == 0) { // Gamestate was not present
    //initialize gameboard
    this->game_board_ = (Array2D)malloc(sizeof(Array2DStruct));
    if(this->game_board_ == NULL){ return false; } //out of memory
    // set all elements in game board to empty candy
    for (int i = 0; i < GetBoardSize(); i++) {
      SetElement(this->game_board_, i, (Array_t)&EMPTY_CANDY);
    }

    //initialize fired state
    this->fired_state_ = (Array2D)malloc(sizeof(Array2DStruct));
    if(this->fired_state_ == NULL){ return false; } //out of memory
    // set fired_state to equal the contents of initial_fired_state
    *(this->fired_state_) = *(this->initial_fired_state_);

    //zero initialize score
    this->score_ = 0;

    //zero initialize extensionoffset
    extension_offset_.reserve(GetNumCols(this->initial_fired_state_));
    for (vector<int>::iterator itr = extension_offset_.begin(); itr != extension_offset_.end(); itr++) {
      *itr = 0;
    }

    //zero initialize moves_made
    this->moves_made_ = 0;
    //call settle on board to populate game_board
    ApplyGravity();

  } else if (return_int == 1) { // Gamestate was present
    // game should be fully initialized
    // perform any matinance if needed
  } else { // error occured when attempting to read game state
    //TODO: signal error somehow
    return false;
  }

  return true;
}

bool GameModel::DeserializeGameDef(json_t* game_instance){
  json_t* json_gamedef;
  int gameid, movesallowed, colors;
  json_t *json_extensioncolor, *json_boardstate;
  Array2D extensioncolor, boardstate;

  json_gamedef = json_object_get(game_instance, "gamedef");
  if (json_gamedef == nullptr) {
    fprintf(stderr, "could not find 'gamedef' in json file");
    return false;
  }

  json_unpack(json_gamedef, "{s:i, s:o, s:o, s:i, s:i}", "gameid", &gameid, "extensioncolor", &json_extensioncolor, "boardstate", &json_boardstate, "movesallowed", &movesallowed, "colors", &colors);

  extensioncolor = DeserializeArray2D(json_extensioncolor, DeserializeCandyFunction);
  boardstate = DeserializeArray2D(json_boardstate, DeserializeIntFunction);
  if(extensioncolor == nullptr){ return false; } // out of memory
  if(boardstate == nullptr){ return false; } // out of memory

  // assign values obtained from json file to this game object
  this->game_id_ = gameid;
  this->extension_board_ = extensioncolor;
  this->initial_fired_state_ = boardstate;
  this->total_moves_ = movesallowed;
  this->num_colors_ = colors;

  return true;
}

// returns values:
//  1: gamestate was found and loaded
//  0: gamestate was not present
// -1: Error occured.
int GameModel::DeserializeGameState(json_t* game_instance) {
  json_t* json_gamestate;
  int movesmade, currentscore;
  json_t *json_boardcandies, *json_boardstate, *json_extensionoffset;
  Array2D boardcandies, boardstate;

  json_gamestate = json_object_get(game_instance, "gamestate");
  if (json_gamestate == NULL) { // gamestate wasn't found
    return 0;
  }

  json_unpack(json_gamestate, "{s:o, s:o, s:i, s:i, s:a}", "boardcandies", &json_boardcandies, "boardstate", &json_boardstate, "movesmade", &movesmade, "currentscore", &currentscore, "extensionoffset", &json_extensionoffset);

  boardstate = DeserializeArray2D(json_boardstate, DeserializeIntFunction);
  boardcandies = DeserializeArray2D(json_boardcandies, DeserializeCandyFunction);
  if(boardstate == nullptr){return -1;} //out of memory
  if(boardcandies == nullptr){return -1;} //out of memory

  // create the extension offset
  size_t idx;
  json_t* value;
  vector<int> extensionoffset;
  extensionoffset.reserve(GetNumCols(boardstate));
  json_array_foreach(json_extensionoffset, idx, value) {
    // add int val from  json_extension_offset
    extensionoffset.push_back((int)(json_number_value(value)));
  }

  // assign values obtained from json file to this game object
  this->game_board_ = boardcandies;
  this->fired_state_ = boardstate;
  this->moves_made_ = movesmade;
  this->score_ = currentscore;
  this->extension_offset_ = extensionoffset;

  return true;
}

Array2D DeserializeArray2D(json_t* serialized_array2d, ElDeserializeFnPtr deserialize_function){
  Array2D array_field;
  json_t* data;
  json_error_t error;
  int rows, cols;
  ElDeserializeFnPtr deserialize_fn;

  deserialize_fn = deserialize_function;

  // allocate space for array2D on heap
  array_field = (Array2D)malloc(sizeof(Array2DStruct));
  if (array_field == NULL) { return nullptr; } // out of memory

  json_unpack(serialized_array2d, "{s:i, s:i, s:o}", "rows", &rows,
              "columns", &cols, "data", &data);

  //insert deserialized values into Array2D
  array_field->num_rows = rows;
  array_field->num_cols = cols;
  array_field->size = (cols * rows);

  array_field->data = (Array_t*)malloc(sizeof(Array_t) * (array_field->size));
  if (array_field == NULL) { return nullptr; } // out of memory

  // Use client-provided function to insert data into array
  deserialize_fn(array_field, data);
  return array_field;
}

/*
extern "C" void FillCandyArray2D(Array2D array, json_t* data){
  json_t* json_value;
  size_t index;
  int color, type;
  Candy* candy_ptr;

  json_array_foreach(data, index, json_value) {
    candy_ptr = (Candy*)malloc(sizeof(Candy));
    if(candy_ptr == NULL){ printf("HA! you ran out of mem and are about to segfault"); }

    json_unpack(json_value, "{ s:i, s:i }", "color", &(candy_ptr->color), "type", &(candy_ptr->type));

    array->data[index] = candy_ptr;
    // printf("\t\t deserialized %lu\n", index);
  }

  json_array_clear(data);
}*/

/*
void GameModel::FillArrayInt2D(Array2D array, json_t* data){
  json_t* value;
  size_t index;
  int el_value;

  json_array_foreach(data, index, value) {
    json_unpack(value, "i", &el_value);
    array->data[index] = (json_t*)((long)el_value);
    // printf("\t\t deserialized %lu\n", index);
  }

  json_array_clear(data);
}*/

void GameModel::SerializeGame(const string &filepath) {
    //TODO
    const char* c_filepath = filepath.c_str();
    bool result;
    result = SerializeGameInstance(c_filepath);
    if(!result){ // something failed
      cout << "Encountered an error while trying to serialize game instance." << endl;
    }
}

bool GameModel::SerializeGameInstance(const char* &filepath) {
  json_t *json_gameinstance, *json_gamedef, *json_gamestate;

  json_gamedef = this->SerializeGameDef();
  json_gamestate = this->SerializeGameState();

  json_gameinstance = json_object();
  json_object_set(json_gameinstance, "gamedef", json_gamedef);
  json_object_set(json_gameinstance, "gamestate", json_gamestate);

  json_dump_file(json_gameinstance, filepath, 0);

  json_decref(json_gamedef);
  json_decref(json_gamestate);
  json_decref(json_gameinstance);
}

json_t* GameModel::SerializeGameDef(void){
  json_t* json_gamedef = json_object();

  // serialize gameid
  json_object_set_new(json_gamedef, "gameid", json_integer(this->game_id_));

  // serialize extensioncolor
  json_t* json_extensioncolor = SerializeArray2D(this->extension_board_, SerializeIntFunction);
  json_object_set(json_gamedef, "extensioncolor", json_extensioncolor);
  json_decref(json_extensioncolor); // may not want these decref statements if the json_gamedef doesn't increment objectref

  // serialize initial boardstate
  json_t* json_initialboardstate = SerializeArray2D(this->initial_fired_state_, SerializeIntFunction);
  json_object_set(json_gamedef, "boardstate", json_initialboardstate);
  json_decref(json_initialboardstate); // may not want these decref statements (see above)

  // serialize movesallowed
  json_object_set_new(json_gamedef, "movesallowed", json_integer(this->total_moves_));

  // serialize colors
  json_object_set_new(json_gamedef, "colors", json_integer(this->num_colors_));

  return json_gamedef;
}

json_t* GameModel::SerializeGameState(void){
  json_t* json_gamedef = json_object();

  // serialize boardcandies
  json_t* json_boardcandies = SerializeArray2D(this->game_board_, SerializeCandyFunction);
  json_object_set(json_gamedef, "boardcandies", json_boardcandies);
  json_decref(json_boardcandies); // may not want these decref statements if the json_gamedef doesn't increment objectref

                                  // serialize boardstate
  json_t* json_boardstate = SerializeArray2D(this->fired_state_, SerializeIntFunction);
  json_object_set(json_gamedef, "boardstate", json_boardstate);
  json_decref(json_boardstate); // may not want these decref statements (see above)

  // serialize movesmade
  json_object_set_new(json_gamedef, "movesmade", json_integer(this->moves_made_));

  // serialize currentscore
  json_object_set_new(json_gamedef, "currentscore", json_integer(this->score_));

  // serialize extensionoffset
  json_t* json_extensionoffset = json_array();
  for (int i : this->extension_offset_) {
    json_array_append_new(json_extensionoffset, json_integer(i));
  }
  json_object_set(json_gamedef, "extensionoffset", json_extensionoffset);
  json_decref(json_extensionoffset);

  return json_gamedef;
}

json_t* SerializeArray2D(Array2D array, ElSerializeFnPtr serialize_function) {
  json_t *json_data, *json_arr;
  ElSerializeFnPtr serialize_fn;

  serialize_fn = serialize_function;
  json_data = serialize_fn(array);

  json_arr = json_object();
  json_object_set_new(json_arr, "rows", json_integer(GetNumRows(array)));
  json_object_set_new(json_arr, "columns", json_integer(GetNumCols(array)));
  json_object_set(json_arr, "data", json_data);

  return json_arr;
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
    if(fired) {
        cout << "Fired successfully!" << endl;
    } else {
        cout << "Didn't fire." << endl;
    }
    return fired;
}

bool GameModel::FindAndFireTemplates(const int &num, const bool &isVertical) {
    bool fired = false;
    int seq_count, color;

    for(int idx = 0; idx < GetBoardSize(); idx++) {
        if(isVertical) {
            //traverse each column, looking for num sized sequence
            //if
        } else {

        }
    }
}

bool GameModel::FindAndFireVerticalMatch(const int &num) {
    bool fired = false;
    int idx = 0;
    int idx, seq_count, color;

    for(int col = 0; col < GetRowLength(); col++) {
        if (seq_count == num) {
            for (int i = 0; i < num; i++) {
                SetCandy(idx, -1, 0);
                AdjustScore(idx);
                idx -= GetRowLength();
            }
            fired = true;
        }
        seq_count = 0;
        color = -2;
        for (int row = 0; row < GetColLength(); row++) {
            idx = ConvertToIdx(row, col);

            if (seq_count == num) {
                for (int i = 0; i < num; i++) {
                    SetCandy(idx, -1, 0);
                    AdjustScore(idx);
                    idx -= GetRowLength();
                }
                seq_count = 0;
                color = -2;
                fired = true;
            }

            if (GetCandyColor(idx) == color) {
                seq_count++;
            } else if (GetCandyColor(idx) == -1) {
                color = -2;
                seq_count = 0;
            } else {
                color = GetCandyColor(idx);
                seq_count = 1;
            }
        }
    }

    if (seq_count == num) {
        for (int i = 0; i < num; i++) {
            SetCandy(idx, -1, 0);
            AdjustScore(idx);
            idx -= GetRowLength();
        }
        fired = true;
    }

    return fired;
}

bool GameModel::FindAndFireHorizontalMatch(const int &num) {
    int idx, seq_count, color;
    bool fired = false;

    for(int row = 0; row < GetColLength(); row++) {
        seq_count = 0;
        color = -2;
        for(int col = 0; col < GetRowLength(); col++) {
            idx = ConvertToIdx(row, col);

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
            fired = true;
        }
    }
    if (seq_count == num) {
        for (int i = 0; i < num; i++) {
            SetCandy(idx, -1, 0);
            AdjustScore(idx);
            idx -= GetRowLength();
        }
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
    }
}

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
    int el_value;

    //Unpack array of Candies from JSON object into the Array2D
    json_array_foreach(data, index, value) {
        CandyPtr candy = (CandyPtr)malloc(sizeof(Candy));
        json_unpack(value, "i", &el_value);
        candy->color = el_value;
        candy->type = 0;
        array->data[index] = (Json_ptr)(candy);
    }
    json_array_clear(data);
}

json_t* SerializeCandyFunction(Array2D array){
  json_t* json_arr = json_array();
  for (int i = 0; i < array->size; i++) {
    json_t* json_candy = json_object();
    json_object_set_new(json_candy, "color", json_integer(((Candy*)array->data[i])->color));
    json_object_set_new(json_candy, "type", json_integer(((Candy*)array->data[i])->type));
    json_array_append_new(json_arr, json_candy);
    json_decref(json_candy);
  }
  return json_arr;
}

json_t* SerializeIntFunction(Array2D array) {
  json_t* json_arr = json_array();
  for (int i = 0; i < array->size; i++) {
    json_array_append_new(json_arr, json_integer((int)array->data[i]));
  }
  return json_arr;
}

// ======================================================== END PRIVATE METHODS
