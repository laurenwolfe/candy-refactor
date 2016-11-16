#include <gtk/gtk.h>
#include <jansson.h>

#include "../hw2/array2d.h"
#include "model.h"

BoardPtr CreateBoard(char *filename) {
    //Create main gameboard struct and allocate space for data array
    BoardPtr gameboard = AllocateBoard();

    //Load array data into Array2D within gameboard
    if(gameboard != NULL) {
        Deserialize(gameboard->array_ptr, filename, &DeserializeFunction);
    }

    return gameboard;
}

// Malloc space for gameboard and its inner Array2D stucture.
// Returns NULL if there isn't enough memory available to allocate,
// Otherwise returns pointer to a new Board.
BoardPtr AllocateBoard(void) {
    BoardPtr gameboard;

    gameboard = (BoardPtr)malloc(sizeof(Board));
    if(gameboard == NULL) return (BoardPtr)NULL;

    gameboard->array_ptr = AllocateArray2D();
    if(gameboard->array_ptr == NULL) return (BoardPtr)NULL;

    // MOVES constant is defined in model.h
    gameboard->moves = MOVES;
    gameboard->selected_idx = NO_CANDY;

    return gameboard;
}

// Frees memory allocated for Board struct and its data Array.
// Returns true if successfully freed, false otherwise.
boolean DestroyBoard(BoardPtr gameboard, payloadFreeFunctionPtr freeFnPtr) {
    boolean array_destroyed;

    if(gameboard == NULL || freeFnPtr == NULL) return false;

    array_destroyed = DestroyArray2D(gameboard->array_ptr, freeFnPtr);

    // Only free gameboard if array is successfully destroyed, otherwise
    // we lose reference to it.
    if(array_destroyed) free(gameboard);

    return array_destroyed;
}

// Set the selected candy by passing its index
void SetSelectedCandy(BoardPtr gameboard, int idx) {
    gameboard->selected_idx = idx;
}

// Get the value currently stored as selected candy's index.
// Returns -1 for no candy selected, index otherwise.
int GetSelectedCandy(BoardPtr gameboard) {
    return gameboard->selected_idx;
}

// Decrement number of moves remaining
void DecrementMoves(BoardPtr gameboard) {
    gameboard->moves--;
}

// Set the number of moves remaining
void SetMoves(BoardPtr gameboard, int moves) {
    gameboard->moves = moves;
}

// Get the number of moves remaining
int GetMoves(BoardPtr gameboard) {
    return gameboard->moves;
}

//Swap the position of two candies
boolean SwapCandy(BoardPtr gameboard, char dir) {
    int idx1, idx2;

    //Validate input, verify that a candy has been selected and moves remain
    if(gameboard == NULL || gameboard->selected_idx == NO_CANDY ||
            gameboard->moves == 0) {
        return false;
    }

    idx1 = gameboard->selected_idx;
    idx2 = gameboard->selected_idx;

    //Get relative index for element in swap direction
    if(dir == 'N') {
        idx2 -= GetColLength(gameboard);
    } else if(dir == 'S') {
        idx2 += GetColLength(gameboard);
    } else if(dir == 'E') {
        idx2++;
    } else if(dir == 'W') {
        idx2--;
    } else {
        return false;
    }

    //Ensure that move is legal
    if(idx2 < 0 || idx2 >= GetSize(gameboard->array_ptr)) {
        return false;
    }

    // Return true if swap is a valid move and executes successfully
    if(IsValidSwap(gameboard, idx1, idx2) &&
            Swap(gameboard->array_ptr, idx1, idx2)) {
        return true;
    }
    return false;
}

//TODO
//This function will evaluate the swap to see if the move fulfills the game rules
boolean IsValidSwap(BoardPtr gameboard, int idx1, int idx2) {
    return true;
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

// Serialize data from the Array2D and write to file
// TODO: Not sure what this return is for, look into it
Json_ptr SerializeFunction(Array2D array) {
    Json_ptr data_arr, value;
    int element;

    data_arr = json_array();

    for(int i = 0; i < array->size; i++) {
        element = (int)array->data[i];
        value = json_integer(element);

        json_array_append_new(data_arr, value);
    }
    return data_arr;
}

// Would free memory in the case of using pointers, but we're storing ints in the array.
// Therefore, this function doesn't need to perform any action.
void FreeFunction(Array_t pointer) {

}

// Returns the height (number of elements) of the Board
int GetRowLength(BoardPtr gameboard) {
    return GetNumRows(gameboard->array_ptr);
}

// Returns the width (number of elements) of the board
int GetColLength(BoardPtr gameboard) {
    return GetNumCols(gameboard->array_ptr);
}

// Get value of candy at the provided index.
// Returns the candy type (represented by int) if
int GetCandy(BoardPtr gameboard, int idx) {
/*
    int val;
    int *ptr;

    ptr = (int *)malloc(sizeof(int *));

    if(gameboard->array_ptr != NULL) { //&& idx < GetSize(gameboard->array_ptr)) {
        GetElement(gameboard->array_ptr, idx, &ptr);
        val = *ptr;
    } else {
        val = NO_CANDY;
    }

    //free(ptr);
    return val;
*/
    int *ptr1, *ptr2;

    ptr1 = (int *)malloc(sizeof(int *));
    ptr2 = (int *)malloc(sizeof(int *));


    ptr1 = (int *)GetEl(gameboard->array_ptr, idx);
    *ptr2 = *(int*)GetEl(gameboard->array_ptr, idx);

    printf("ptr1: %p", ptr1);
    printf("*ptr1: %d", *ptr1);
    printf("ptr2: %p", ptr2);
    printf("*ptr2: %d", *ptr2);

    return 0;

}