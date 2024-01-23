#include <stdio.h>
#include <stdbool.h>

#define RESULT_SIZE 25
#define BOARD_SIZE 5

int xmoves[] = {2, 1, -1, -2, -2, -1, 1, 2};//possible x moves
int ymoves[] = {1, 2, 2, 1, -1, -2, -2, -1};//possible y moves
int count = 0;//number of tours found
//checks if in bounds and spot is not found (-1 means not found)
bool canMove(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]) {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == -1);
}
void recursiveKnight(int x, int y, int moveCount, int board[BOARD_SIZE][BOARD_SIZE],int result[RESULT_SIZE]) {
    int i;
    //printing here if the moveCount is 25
    if (moveCount == BOARD_SIZE * BOARD_SIZE) {
        //IF I WERE TO PRINT THE BOARD ARRAY HERE IT WOULD BE THE CHALLENGE TO PRINT SPACES IN ORDER VISITED
        for(i = 0; i < RESULT_SIZE; i++){
            printf("%d",result[i]);
            if(i < RESULT_SIZE-1){
                printf(",");
            }
        }
        printf("\n");
        count++;
        return;
    }
    for (int k = 0; k < 8; k++) {//check over every possible move from the square
        int nextX = x + xmoves[k];
        int nextY = y + ymoves[k];
        if (canMove(nextX, nextY, board) ) {
            result[moveCount] = nextX+(nextY*5) + 1;
            board[nextX][nextY] = moveCount; //set to found
            recursiveKnight(nextX, nextY, moveCount + 1, board,result);
            board[nextX][nextY] = -1; // to allow backwards movement in the event of a dead end
        }
    }
}
void knightTour() {
    int result[RESULT_SIZE];//this is where the result will be held thatll get printed
    int board[BOARD_SIZE][BOARD_SIZE];
    int i,j,h,k;
    //yucky 4 for loops but need to init for every point on board
    for (h = 0; h < BOARD_SIZE; h++)
        for (k = 0; k < BOARD_SIZE; k++){           
        //init board to set all spots to be allowed to be picked
        for (i = 0; i < BOARD_SIZE; i++)
            for (j = 0; j < BOARD_SIZE; j++){
                board[i][j] = -1;
            }
        //setting current spot to correct number
        board[h][k] = h+(k*5) + 1;
        result[0] = h+(k*5) + 1;
        //start algo on the current spot
        recursiveKnight(h, k, 1, board,result);
    }
}
int main(int argc, char *argv[]) {
    knightTour(); //start it!
    return 0;
}
