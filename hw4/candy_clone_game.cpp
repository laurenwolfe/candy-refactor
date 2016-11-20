// #include "candy_clone_game.h"
#include "../jansson/include/jansson.h"
// #include "../hw2/array2d.h"
#include <vector>
#include <string>

extern "C" typedef void(*ElDeserializeFnPtr)(Array2D array, json_t* data);
extern "C" typedef json_t*(*ElSerializeFnPtr)(Array2D array);
using std::string;
using std::vector;

/*
Candy clone game is a class to simulate yet another candy crush knock-off

** expand class description if time permits **
*/

// frees an int from an array2D
// does nothing because ints stored directly in array2D without pointers
extern "C" static void IntFreeFunction(Array_t int_ptr) {}

// frees an candy from an array2D
extern "C" static void CandyFreeFunction(Array_t candy_ptr) {
  free(candy_ptr);
}

// --- Serialize Functions ----------------------------------------------------
// serializes an entire game instance including the gamedef and the gamestate
bool SerializeGameInstance(char* filepath) {
  json_t *json_gameinstance, *json_gamedef, *json_gamestate;
  
  json_gamedef = this.SerializeGamedef();
  json_gamestate = this.SerializeGameState();

  json_gameinstance = json_object();
  json_object_set(json_gameinstance, "gamedef", json_gamedef);
  json_object_set(json_gameinstance, "gamestate", json_gamestate);

  json_dump_file(json_gameinstance, filepath, 0);

  json_decref(json_gamedef);
  json_decref(json_gamestate);
  json_decref(json_gameinstance);
}

json_t* SerializeCandyArray(Array2D array) {
  json_t* json_arr = json_array();
  for (int i = 0; i < array->size; i++) {
    json_t* json_candy = json_object();
    json_object_set_new(json_candy, "color", json_integer(array->data[i]->color));
    json_object_set_new(json_candy, "type", json_integer(array->data[i]->type));
    json_array_append_new(json_arr, json_candy);
    json_decref(json_candy);
  }
  return json_arr;
}

json_t* SerializeIntArray(Array2D array) {
  json_t* json_arr = json_array();
  for (int i = 0; i < array->size; i++) {
    json_array_append_new(json_arr, json_integer(array->data[i]));
  }
  return json_arr;
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

json_t* SerializeGameDef(void) {
  json_t* json_gamedef = json_object();

  // serialize gameid
  json_object_set_new(json_gamedef, "gameid", json_integer(this->gameid));

  // serialize extensioncolor
  json_t* json_extensioncolor = SerializeArray2D(this->extensioncolor, SerializeIntArray);
  json_object_set(json_gamedef, "extensioncolor", json_extensioncolor);
  json_decref(json_extensioncolor); // may not want these decref statements if the json_gamedef doesn't increment objectref

  // serialize boardstate
  json_t* json_boardstate = SerializeArray2D(this->boardstate, SerializeIntArray);
  json_object_set(json_gamedef, "boardstate", json_boardstate);
  json_decref(json_boardstate); // may not want these decref statements (see above)

  // serialize movesallowed
  json_object_set_new(json_gamedef, "movesallowed", json_integer(this->movesallowed));

  // serialize colors
  json_object_set_new(json_gamedef, "colors", json_integer(this->colors));

  return json_gamedef;
}

json_t* SerializeGameState(void) {
  json_t* json_gamedef = json_object();

  // serialize boardcandies
  json_t* json_boardcandies = SerializeArray2D(this->boardcandies, SerializeCandyArray);
  json_object_set(json_gamedef, "boardcandies", json_boardcandies);
  json_decref(json_boardcandies); // may not want these decref statements if the json_gamedef doesn't increment objectref

  // serialize boardstate
  json_t* json_boardstate = SerializeArray2D(this->boardstate, SerializeIntArray);
  json_object_set(json_gamedef, "boardstate", json_boardstate);
  json_decref(json_boardstate); // may not want these decref statements (see above)

  // serialize movesmade
  json_object_set_new(json_gamedef, "movesmade", json_integer(this->movesmade));

  // serialize currentscore
  json_object_set_new(json_gamedef, "currentscore", json_integer(this->currentscore));

  // serialize extensionoffset
  json_t* json_extensionoffset = json_array();
  for(int i = 0; i < this)
  json_decref(json_extensionoffset);
  return json_gamedef;
}

// ---------------------------------------------------------------------------------------

// --- Deserialize Functions -------------------------------------------------------------


// fills the contents of an array2d with the contents of a json int array
extern "C" static void FillArray2D(Array2D array, json_t* data) {
  json_t* value;
  size_t index;
  int el_value;

  json_array_foreach(data, index, value) {
    json_unpack(value, "i", &el_value);
    array->data[index] = (json_t*)((long)el_value);
    // printf("\t\t deserialized %lu\n", index);
  }

  json_array_clear(data);
}

// fills the contents of a array2D with the contents of a json candy array
extern "C" static void FillCandyArray2D(Array2D array, json_t* data) {
  json_t* json_value;
  size_t index;
  int color, type;
  Candy* candy;

  json_array_foreach(data, index, json_value) {
    candy = (Candy*)malloc(sizeof(Candy));

    json_unpack(json_value, "{ s:i, s:i }", "color", &(candy->color), "type", &(candy->type));

    array->data[index] = candy;
    // printf("\t\t deserialized %lu\n", index);
  }

  json_array_clear(data);
}

extern "C" static Array2D DeserializeArray2D(json_t* serialized_array2d, ElDeserializeFnPtr deserialize_function) {
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

  array_field->data = (Array_t)malloc(sizeof(Array_t) * (array_field->size));
  if (array_field == NULL) { return nullptr; } // out of memory

  //Use client-provided function to insert data into array
  deserialize_fn(array_field, data);
  return array_field;
}

static bool DeserializeGameDef(json_t* game_instance) {
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

  extensioncolor = DeserializeArray2D(json_extensioncolor, FillArray2D);
  boardstate = DeserializeArray2D(json_boardstate, FillArray2D);

  return true;
}

static bool DeserializeGameState(json_t* game_instance) {
  json_t* json_gamestate;
  int movesmade, currentscore;
  json_t *json_boardcandies, *json_boardstate, *json_extensionoffset;
  Array2D boardcandies, boardstate;
  vector<int> extensionoffset;

  json_gamestate = json_object_get(game_instance, "gamestate");
  if (json_gamestate == NULL) { // gamestate wasn't found
    return false;
  }

  json_unpack(json_gamestate, "{s:o, s:o, s:i, s:i, s:a}", "boardcandies", &json_boardcandies, "boardstate", &json_boardstate, "movesmade", &movesmade, "currentscore", &currentscore, "extensionoffset", &json_extensionoffset);

  boardstate = DeserializeArray2D(json_boardstate, FillArray2D);
  boardcandies = DeserializeArray2D(json_boardcandies, FillCandyArray2D);

  size_t idx;
  json_t* value;
  extensionoffset.reserve(GetNumCols(boardstate));
  json_array_foreach(json_extensionoffset, idx, value) {
    // add int val from  json_extension_offset
    extensionoffset.push_back((int)(json_number_value(value)));
  }
  return true;
}
// ----------------------------------------------------------------------------------------
/*
static bool deserialize(char* filename) {
  json_t *json_gameinstance, *json_gamedef, *json_gamestate;
  json_error_t error;
  void* json_iter;

  // fields for gamedef
  int gameid, movesallowed, colors;
  json_t *json_extensioncolor, *json_init_boardstate;
  Array2D extensioncolor, init_boardstate;

  // fields for gamestate
  int movesmade, currentscore;
  json_t *json_boardcandies, *json_curr_boardstate, *json_extensionoffset;
  Array2D boardcandies, curr_boardstate;

  json_gameinstance = json_load_file(filename, 0, &error); // get json object

  // error reading json file
  if (!json_gameinstance) {
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return;
  }

  json_iter = json_object_iter_at(json_gameinstance, "gamedef");
  if(json_iter == NULL){
    fprintf(stderr, "could not find 'gamedef' in json file");
    return;
  }
  json_gamedef = json_object_iter_value(json_iter);
  // define values for gamedef
  json_unpack(json_gamedef, "{s:i, s:o, s:o, s:i, s:i}", "gameid", &gameid, "extensioncolor", &json_extensioncolor, "boardstate", &json_init_boardstate, "movesallowed", &movesallowed, "colors", &colors);

  // deserialize extensioncolor and boardstate
  Deserialize_json_obj(&extensioncolor, json_extensioncolor); // TODO: define deserialize fn ptr for both
  Deserialize_json_obj(&init_boardstate, json_init_boardstate);

  // get next object from json iter
  json_iter = json_object_iter_at(json_gameinstance, "gamestate");
  if (json_iter != NULL) {
    // gameinstance had a gamestate
    json_gamestate = json_object_iter_value(json_iter);
    json_unpack(json_gamestate, "{s:o, s:o, s:i, s:i, s:a}", "boardcandies", &json_boardcandies, "boardstate", &json_curr_boardstate, "movesmade", &movesmade, "currentscore", &currentscore, "extensionoffset", &json_extensionoffset);

    Deserialize_json_obj(&boardcandies, json_boardcandies);
    Deserialize_json_obj(&curr_boardstate, json_curr_boardstate);



  }





  // create the gamedef
  json_gamedef =

  if(!json_object_iter_next(json_iter, gameinstance))

}
*/
//// constructor
//CandyCloneGame::CandyCloneGame() {
//
//}
//
//// destructor
//CandyCloneGame::~CandyCloneGame() {
//
//}
//
//// swaps the currently selected element in the specified direction
//void swap(direction) {
//  // call swap on board
//  // if (swap is illegal) {
//    // do nothing
//  // else if (swap does not result in template)
//    // undo swap
//    // return some val
//  // if swap does result in template
//    // decrement the remaining moves
//    // settleBoard();
//}
//
//void FindTemplate() {
//  // search through the color board in row column order from left to right
//
//  // for all cells in color_board:
//    // if( cell_is_vFour_pattern() ){
//      // signal found pattern
//      // goto end of function
//
//  // for all cells in color_board:
//    // if( cell_is_hFour_pattern() ){
//      // signal found pattern
//      // goto end of function
//
//  // for all cells in color_board:
//    // if( cell_is_vThree_pattern() ){
//      // signal found pattern
//      // goto end of function
//
//  // for all cells in color_board:
//    // if( cell_is_hThree_pattern() ){
//      // signal found pattern
//      // goto end of function
//
//  // if (a cell matched a template)
//    // return index template was found as well as the type of template it matched
//    // most likely through a return parameter
//}
//
//private bool CellIsVFourTemplate(int idx) {
//  // if cell does not have four above
//    // return false
//  // if (cell.row + 1 != cell.color) {
//    // return false;
//  // if (cell.row + 2 != cell.color) {
//    // return false;
//  // if (cell.row + 3 != cell.color) {
//    // return false;
//  // else
//    // return true;
//}
//
//private bool CellIsHFourTemplate(int idx) {
//  // if cell does not have four to right
//    // return false
//  // if (cell.col + 1 != cell.color) {
//    // return false;
//  // if (cell.col + 2 != cell.color) {
//    // return false;
//  // if (cell.col + 3 != cell.color) {
//    // return false;
//  // else
//    // return true;
//}
//
//private bool CellIsVThreeTemplate(int idx) {
//  // if cell does not have three above
//    // return false
//  // if (cell.row + 1 != cell.color) {
//    // return false;
//  // if (cell.row + 2 != cell.color) {
//    // return false;
//  // else
//    // return true;
//}
//
//private bool CellIsHThreeTemplate(int idx) {
//  // if cell does not have three to right
//    // return false
//  // if (cell.col + 1 != cell.color) {
//    // return false;
//  // if (cell.col + 2 != cell.color) {
//    // return false;
//  // else
//    // return true;
//}
//
//// assumes that idx and template are both valid
//void ExplodeCandy(int idx) {
//  // clear the color at idx in the color board
//  // decrement the count at idx in the score_board if count >0
//  // idx and following the specified template
//}
//
//void ExplodeTemplate(String tmplate, int idx){
//  // for each square in template
//    // explodeCandy(square)
//}
//
//void settle() {
//  // GravityAll();
//
//
//  // for all squares in board
//    // if square has template
//      // explodeTemplate(template type, square)
//  // while( findtemplate() is successful ) {
//    // ExplodeCandy( cell_from_found_template, template);
//
//    // if score = 0
//      // game_won = true;
//
//    // GravityAll();
//}
//
//void GravityAll() {
//  // for each cell in gameboard:
//    // if(cell is empty)
//      // GravitySub(cell, 0);
//}
//
//// recursive function that fills in square with next non-empty square above
//candy GravitySquare(int idx, int lvl) {
//  // if (idx is outside game board) {
//    // get next candy from this col in extended_board
//    // return candy from extended_board
//
//  // if (this idx has a candy in it){
//    // return_candy = getcandy(idx)
//    // clearCandy(idx)
//    // return return_candy;
//  
//  // if (this idx is empty)
//    // result_candy = GravitySub(cell above idx, lvl+1);
//    // if(lvl == 0) {
//      // we are back at the origional cell
//      // idx.setCandy(result_candy)
//      // return null; // (we are finished)
//    // else 
//      // continue to propagate candy down
//      // return result_candy
//  }
//
//
//
//}

