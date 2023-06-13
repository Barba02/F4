#include "game.h"
#include <stdio.h>
#include <unistd.h>

//print game matrix
void print_game(int rows, int cols, int mat[rows][cols], char  p1_sign, char p2_sign) {
    // print cols numeration
    printf("\n");
    for (int i = 0; i < cols; i++)
        printf("  %d ", i+1);
    printf("\n");
    // print matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("|");
            if (mat[i][j] == 0)
                printf("   ");
            else
                printf(" %c ", (mat[i][j] == 1) ? p1_sign : p2_sign);
            if (j == cols-1)
                printf("|\n");
        }
    }
    // print closing line
    for (int i = 0; i < cols; i++)
        printf("----");
    printf("-\n");
}

void F4_game(game_t *game_data, int matrix_game[game_data->rows][game_data->cols]) {
    int choice, player, error = 0;
    while (1) {
        // print matrix game
        print_game(game_data->rows, game_data->cols, matrix_game, game_data->client1_sign, game_data->client2_sign);
        player = (getpid() == game_data->client1_pid) ? 1 : 2;
        // choice of column
        do {
            if (error)
                printf("Choosen column must be in the game range and not full\n");
            printf("Insert column number: ");
            scanf("%d", &choice);
            error = (choice < 1 || choice > game_data->cols || play(game_data, matrix_game, choice, player) == -1);
        } while (error);
    }
}

int play(game_t *game_data, int matrix_game[game_data->rows][game_data->cols], int choice, int player){
    //Scorro la colonna scelta a partire dal basso
    for(int i=game_data->rows-1;i>=0;i--){
        if(matrix_game[i][choice-1]==0){
            matrix_game[i][choice-1]=player;
            return 0;
        }
        else
            continue;
    }
    return -1;
}

int check_win(game_t *game_data, int matrix_game[game_data->rows][game_data->cols]){
    // Controlla vittoria orizzontale
    for (int row = 0; row < game_data->rows; row++) {
        for (int col = 0; col < game_data->cols - 3; col++) {
            if (matrix_game[row][col] != 0 && matrix_game[row][col] == matrix_game[row][col + 1] && matrix_game[row][col] == matrix_game[row][col + 2] && matrix_game[row][col] == matrix_game[row][col + 3]) {
                return 1; // Vittoria
            }
        }
    }

    // Controlla vittoria verticale
    for (int row = 0; row < game_data->rows - 3; row++) {
        for (int col = 0; col < game_data->cols; col++) {
            if (matrix_game[row][col] != 0 && matrix_game[row][col] == matrix_game[row + 1][col] && matrix_game[row][col] == matrix_game[row + 2][col] && matrix_game[row][col] == matrix_game[row + 3][col]) {
                return 1; // Vittoria
            }
        }
    }

    // Controlla vittoria diagonale (verso destra)
    for (int row = 0; row < game_data->rows - 3; row++) {
        for (int col = 0; col < game_data->cols - 3; col++) {
            if (matrix_game[row][col] != 0 && matrix_game[row][col] == matrix_game[row + 1][col + 1] && matrix_game[row][col] == matrix_game[row + 2][col + 2] && matrix_game[row][col] == matrix_game[row + 3][col + 3]) {
                return 1; // Vittoria
            }
        }
    }

    // Controlla vittoria diagonale (verso sinistra)
    for (int row = 3; row < game_data->rows; row++) {
        for (int col = 0; col < game_data->cols - 3; col++) {
            if (matrix_game[row][col] != 0 && matrix_game[row][col] == matrix_game[row - 1][col + 1] && matrix_game[row][col] == matrix_game[row - 2][col + 2] && matrix_game[row][col] == matrix_game[row - 3][col + 3]) {
                return 1; // Vittoria
            }
        }
    }

    return 0; // Nessun giocatore ha vinto
}

