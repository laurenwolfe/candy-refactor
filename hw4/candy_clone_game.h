#ifndef _CANDY_CLONE_GAME_H_
#define _CANDY_CLONE_GAME_H_

extern "C" {
  #include "../hw2/array2d.h"
  #include <jansson.h>
}

#include <string>
#include <vector>


namespace candyclonegame {

class CandyCloneGame {
  public:
    // constructor, copy constructor destructor
    CandyCloneGame(int gameid);
    // CandyCloneGame(const CandyCloneGame &other); // may just want to disable copy constructor
    ~CandyCloneGame();

    // getters, setters
    int GetGameId() const { return this->game_id_ };

    // game functions
    void SettleBoard();
    void ApplyGravity();
    void FindPattern();
    void Swap(

  private:
  // instance variables
    int game_id_;
    Array2D extension_board_; 
    Array2D point_board_; // stores point information for gameboard
    Array2D candy_board_; // stores candy information for gameboard
    std::vector<int> candy_colors_; // array containing the allowable colors of candy
    int moves_allowed_, moves_taken_, moves_remaining_; 
    int score_; // (initial_board_state_total) - (current_board_state_total)
    std::array<int> extension_tracker_;
    static int MAX_NUM_STEPS = 1000; 

}; // class CandyCloneGame

}

#endif // !_CANDY_CLONE_GAME_H_

