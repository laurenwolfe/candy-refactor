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
bool SerializeGameInstance(const char* filepath) {
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
  json_object_set_new(json_gamedef, "gameid", json_integer(this->game_id_));

  // serialize extensioncolor
  json_t* json_extensioncolor = SerializeArray2D(this->extension_board_, SerializeIntArray);
  json_object_set(json_gamedef, "extensioncolor", json_extensioncolor);
  json_decref(json_extensioncolor); // may not want these decref statements if the json_gamedef doesn't increment objectref

  // serialize initial boardstate
  json_t* json_initialboardstate = SerializeArray2D(this->initial_fired_state_, SerializeIntArray);
  json_object_set(json_gamedef, "boardstate", json_initialboardstate);
  json_decref(json_initialboardstate); // may not want these decref statements (see above)

  // serialize movesallowed
  json_object_set_new(json_gamedef, "movesallowed", json_integer(this->total_moves_));

  // serialize colors
  json_object_set_new(json_gamedef, "colors", json_integer(this->num_colors_));

  return json_gamedef;
}

json_t* SerializeGameState(void) {
  json_t* json_gamedef = json_object();

  // serialize boardcandies
  json_t* json_boardcandies = SerializeArray2D(this->game_board_, SerializeCandyArray);
  json_object_set(json_gamedef, "boardcandies", json_boardcandies);
  json_decref(json_boardcandies); // may not want these decref statements if the json_gamedef doesn't increment objectref

  // serialize boardstate
  json_t* json_boardstate = SerializeArray2D(this->fired_state_, SerializeIntArray);
  json_object_set(json_gamedef, "boardstate", json_boardstate);
  json_decref(json_boardstate); // may not want these decref statements (see above)

  // serialize movesmade
  json_object_set_new(json_gamedef, "movesmade", json_integer(this->moves_made_));

  // serialize currentscore
  json_object_set_new(json_gamedef, "currentscore", json_integer(this->score_));

  // serialize extensionoffset
  //TODO:
  json_t* json_extensionoffset = json_array();
  for (int i : this->extension_offset_) {
    json_array_append_new(json_extensionoffset, i);
  }
  json_object_set(json_gamedef, "extensionoffset", json_extensionoffset);
  json_decref(json_extensionoffset);

  return json_gamedef;
}

// ----------------------------------------------------------------------------

// --- Deserialize Functions --------------------------------------------------


// fills the contents of an array2d with the contents of a json int array
void FillArrayInt2D(Array2D array, json_t* data) {
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
void FillCandyArray2D(Array2D array, json_t* data) {
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
}

Array2D DeserializeArray2D(json_t* serialized_array2d, ElDeserializeFnPtr deserialize_function) {
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

bool DeserializeGameDef(json_t* game_instance) {
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
  if(extensioncolor == nullptr){ return false; } // out of memory
  if(boardstate == nullptr){ return false; } // out of memory

  // assign values obtained from json file to this game object
  this->game_id_ = gameid;
  this->extension_board_ = extensioncolor;
  this->initial_fired_state_ = boardstste;
  this->total_moves_ = movesallowed;
  this->num_colors_ = colors;

  return true;
}

// returns values:
//  1: gamestate was found and loaded
//  0: gamestate was not present
// -1: Error occured.
int DeserializeGameState(json_t* game_instance) {
  json_t* json_gamestate;
  int movesmade, currentscore;
  json_t *json_boardcandies, *json_boardstate, *json_extensionoffset;
  Array2D boardcandies, boardstate;

  json_gamestate = json_object_get(game_instance, "gamestate");
  if (json_gamestate == NULL) { // gamestate wasn't found
    return 0;
  }

  json_unpack(json_gamestate, "{s:o, s:o, s:i, s:i, s:a}", "boardcandies", &json_boardcandies, "boardstate", &json_boardstate, "movesmade", &movesmade, "currentscore", &currentscore, "extensionoffset", &json_extensionoffset);

  boardstate = DeserializeArray2D(json_boardstate, FillArray2D);
  boardcandies = DeserializeArray2D(json_boardcandies, FillCandyArray2D);
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
  this->fired_state_ = boardstste;
  this->moves_made_ = movesmade;
  this->score_ = currentscore;
  this->extension_offset_ = extensionoffset;

  return true;
}
// ----------------------------------------------------------------------------
