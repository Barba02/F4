#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "game.h"
#include "semaphores.h"

//print game matrix
void print_game(int rows, int cols, int mat[rows][cols], char p1_sign, char p2_sign) {
    // print cols numeration
    printf("\n");
    for (int i = 0; i < cols; i++)
        printf("  %d ", i+1);
    printf("\n");
    // print matrix
    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < cols; j++) {
            if (mat[i][j] == 0)
                printf("   |");
            else
                printf(" %c |", (mat[i][j] == 1) ? p1_sign : p2_sign);
        }
        printf("\n");
    }
    // print closing line
    for (int i = 0; i < cols; i++)
        printf("----");
    printf("-\n");
}

void F4_game(game_t *game_data, int matrix_game[game_data->rows][game_data->cols],int semid) {
    printf("Start game\n");
    srand(time(NULL));
    int choice, player, error = 0;
    while (1) {
        player = (getpid() == game_data->client1_pid) ? 1 : 2;
        //
        semOp(semid,player-1,-1);
        // print matrix game
        print_game(game_data->rows, game_data->cols, matrix_game, game_data->client1_sign, game_data->client2_sign);
        // column choice by bot
        if(game_data->autoplay && getpid() == game_data->client2_pid){
            do {
                if (error)
                    printf("Choosen column must be in the game range and not full\n");
                choice = (rand() % game_data->cols) + 1;
                error = (choice < 1 || choice > game_data->cols || play(game_data, matrix_game, choice, player) == -1);
            } while (error);
        }
        // column choice by human client
        else{
            do {
                if (error)
                    printf("Choosen column must be in the game range and not full\n");
                printf("Insert column number: ");
                scanf("%d", &choice);
                error = (choice < 1 || choice > game_data->cols || play(game_data, matrix_game, choice, player) == -1);
            } while (error);
        }
        semOp(semid,player%2,1);
    }
}

int play(game_t *game_data, int matrix_game[game_data->rows][game_data->cols], int choice, int player) {
    // user input human friendly
    choice--;
    // tracing last player
    game_data->last_player = player;
    // loop through the column from bottom to find first free row
    for (int i = game_data->rows-1; i >= 0; i--) {
        if (matrix_game[i][choice] == 0) {
            matrix_game[i][choice] = player;
            game_data->n_played++;
            return 0;
        }
    }
    return -1;
}

int check_win(int rows, int cols, int matrix_game[rows][cols]) {
    // check horizontal
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols - 3; col++) {
            if (matrix_game[row][col] != 0 &&
                matrix_game[row][col] == matrix_game[row][col + 1] &&
                matrix_game[row][col] == matrix_game[row][col + 2] &&
                matrix_game[row][col] == matrix_game[row][col + 3])
                return 1;
        }
    }
    // check vertical
    for (int row = 0; row < rows - 3; row++) {
        for (int col = 0; col < cols; col++) {
            if (matrix_game[row][col] != 0 &&
                matrix_game[row][col] == matrix_game[row + 1][col] &&
                matrix_game[row][col] == matrix_game[row + 2][col] &&
                matrix_game[row][col] == matrix_game[row + 3][col])
                return 1;
        }
    }
    // check diagonal left to right
    for (int row = 0; row < rows - 3; row++) {
        for (int col = 0; col < cols - 3; col++) {
            if (matrix_game[row][col] != 0 &&
                matrix_game[row][col] == matrix_game[row + 1][col + 1] &&
                matrix_game[row][col] == matrix_game[row + 2][col + 2] &&
                matrix_game[row][col] == matrix_game[row + 3][col + 3])
                return 1;
        }
    }
    // check diagonal rigth to left
    for (int row = 3; row < rows; row++) {
        for (int col = 0; col < cols - 3; col++) {
            if (matrix_game[row][col] != 0 &&
                matrix_game[row][col] == matrix_game[row - 1][col + 1] &&
                matrix_game[row][col] == matrix_game[row - 2][col + 2] &&
                matrix_game[row][col] == matrix_game[row - 3][col + 3])
                return 1;
        }
    }
    // nobody win
    return 0;
}
