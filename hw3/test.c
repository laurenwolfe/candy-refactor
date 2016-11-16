#include <gtk/gtk.h>
#include <jansson.h>

#include "model.h"

int main(int argc, char *argv[]) {
    BoardPtr board;

    board = CreateBoard("test.json");

    int *candy1 = (int *)malloc(sizeof(int *));
    int *candy2 = (int *)malloc(sizeof(int *));

    GetCandy(board, 0, candy1);
    GetCandy(board, 1, candy2);

    printf("Candy 1: %d\n", *candy1);
    printf("Candy 2: %d\n", *candy2);

    Swap(board->array_ptr, 0, 1);

    GetCandy(board, 0, candy1);
    GetCandy(board, 1, candy2);

    printf("Candy 1: %d\n", *candy1);
    printf("Candy 2: %d\n", *candy2);

    free(candy1);
    free(candy2);

    return 0;
}