boolean FindVFourMatch() {
    int row, col, row_length, col_length;

    row_length = 8;
    col_length = 11;

    while(row < row_length) {
        int curr_type = -2;
        col = 0;
        int count = 0;

        while(col < col_length) {
            if (col > col_length - 4) {
                //exit the while loop, essentially, because there's no point in
                // continuing to check as there aren't enough elements to match.
                col = col_length;
            } else if(count == 4) {
                //fire candies from this index and three lower indexes in the column

            }else if(candy[idx] != curr_type) {
                //reset check -- don't refire fired candies, so ignore -1 candy color values
                if(candy[idx] == -1) {
                    curr_type = -2;
                } else {
                    curr_type = candy[idx];
                }
                count = 0;
            } else {
                //found another candy, so increment
                count++;
            }
            col++;
        }
    }
}