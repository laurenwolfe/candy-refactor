#include <gtk/gtk.h>
#include <jansson.h>

#include "model.h"

int main(int argc, char *argv[]) {
    BoardPtr board;

    board = CreateBoard("test.json");

    int *candy1 = (int *) malloc(sizeof(int *));

    GetCandy(board, 0, candy1);

    printf("\nCandy 1: %d\n", *candy1);

    //printf("\nCandy 2: %d\n", GetCandy(board, 1));

    //Swap(board->array_ptr, 0, 1);

    //printf("Swapped candy 1: %d\n", GetCandy(board, 0));
    //printf("Swapped candy 2: %d\n", GetCandy(board, 1));

    return 0;
}