#include "candy_clone_game.h"

/*
Candy clone game is a class to simulate yet another candy crush knock-off

** expand class description if time permits **
*/


// parses a json file to reconstruct a game instance
// may include a game state as well.
void serialize() {

}

// converts the current game instance and game state into a json file
void deserialize() {

}

// constructor
CandyCloneGame::CandyCloneGame() {

}

// destructor
CandyCloneGame::~CandyCloneGame() {

}

// swaps the currently selected element in the specified direction
void swap(direction) {
  // call swap on board
  // if (swap is illegal) {
    // do nothing
  // else if (swap does not result in template)
    // undo swap
    // return some val
  // if swap does result in template
    // decrement the remaining moves
    // for all candy in template:
      // explode_candy(candy);
    // after loop if finished and all candies exploded
    // settleBoard();
}

void FindTemplate() {
  // search through the color board in row column order from left to right

  // for all cells in color_board:
    // if( cell_is_vFour_pattern() ){
      // signal found pattern
      // goto end of function

  // for all cells in color_board:
    // if( cell_is_hFour_pattern() ){
      // signal found pattern
      // goto end of function

  // for all cells in color_board:
    // if( cell_is_vThree_pattern() ){
      // signal found pattern
      // goto end of function

  // for all cells in color_board:
    // if( cell_is_hThree_pattern() ){
      // signal found pattern
      // goto end of function

  // if (a cell matched a template)
    // return index template was found as well as the type of template it matched
    // most likely through a return parameter
}

private bool CellIsVFourTemplate(int idx) {
  // if cell does not have four above
    // return false
  // if (cell.row + 1 != cell.color) {
    // return false;
  // if (cell.row + 2 != cell.color) {
    // return false;
  // if (cell.row + 3 != cell.color) {
    // return false;
  // else
    // return true;
}

private bool CellIsHFourTemplate(int idx) {
  // if cell does not have four to right
    // return false
  // if (cell.col + 1 != cell.color) {
    // return false;
  // if (cell.col + 2 != cell.color) {
    // return false;
  // if (cell.col + 3 != cell.color) {
    // return false;
  // else
    // return true;
}

private bool CellIsVThreeTemplate(int idx) {
  // if cell does not have three above
    // return false
  // if (cell.row + 1 != cell.color) {
    // return false;
  // if (cell.row + 2 != cell.color) {
    // return false;
  // else
    // return true;
}

private bool CellIsHThreeTemplate(int idx) {
  // if cell does not have three to right
    // return false
  // if (cell.col + 1 != cell.color) {
    // return false;
  // if (cell.col + 2 != cell.color) {
    // return false;
  // else
    // return true;
}

// assumes that idx and template are both valid
void ExplodeCandy(int idx, string template) {
  // removes colors from the grid starting at cell
  // idx and following the specified template
}

void settle() {
  // GravityAll();

  // while( findtemplate() is successful ) {
    // ExplodeCandy( cell_from_found_template, template);
    // GravityAll();
}

void GravityAll() {

}

void GravitySub() {

}