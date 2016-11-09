#ifndef _GAMEBOARD_H_
#define _GAMEBOARD_H_

#include "../hw2/array2d.h"

#define MOVES 30
#define NO_CANDY (-1)
//#define N No

typedef struct board {
    Array2D array_ptr;
    int selected_idx;
    int moves;
} *BoardPtr, Board;

BoardPtr CreateBoard(char *filename);
BoardPtr AllocateBoard(void);
boolean DestroyBoard(BoardPtr gameboard, payloadFreeFunctionPtr freeFnPtr);

void SetSelectedCandy(BoardPtr gameboard, int idx);
int GetSelectedCandy(BoardPtr gameboard);
boolean SwapCandy(BoardPtr gameboard, char dir);
boolean IsValidSwap(BoardPtr gameboard, int idx1, int idx2);

int GetMoves(BoardPtr gameboard);
void DecrementMoves(BoardPtr gameboard);
void SetMoves(BoardPtr gameboard, int moves);

void DeserializeFunction(Array2D array, Json_ptr data);
Json_ptr SerializeFunction(Array2D array);
void FreeFunction(Array_t pointer);

int GetRowLength(BoardPtr gameboard);
int GetColLength(BoardPtr gameboard);
int GetCandy(BoardPtr gameboard, int idx);

#endif // _GAMEBOARD_H_