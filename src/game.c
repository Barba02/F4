#include "game.h"
#include <stdio.h>
#include <unistd.h>

//print game matrix
void print_game(int rows, int cols, int mat[rows][cols], char  p1_sign, char p2_sign){
    for(int i=0;i<=rows;i++){
        for(int j=0;j<cols;j++){
            // print the last row
            if(i==rows){
                printf("____");
            }
            else{
                printf("|");
                switch (mat[i][j]){
                    case 0:
                        printf("   ");
                        break;
                    case 1:
                        printf(" %c ",p1_sign);
                        break;
                    case 2:
                        printf(" %c ",p2_sign);
                        break;
                }
                // print '|' for last column
                if(j==cols-1)
                    printf("|");
            }
        }
        printf("\n");
    }
}

void F4_game(game_t *game_data,int matrix_game[game_data->rows][game_data->cols]) {
    int choice;
    while (1){
        // print matrix game
        print_game(game_data->rows, game_data->cols, matrix_game, game_data->client1_sign, game_data->client2_sign);
        // choice of column
        do{
            printf("Insert col [1,%i]: ",game_data->cols);
            scanf("%i",&choice);
        }
        while(choice<1 || choice>game_data->cols);
        // get the play (player1 or player2)
        if(getpid() == game_data->client1_pid)
            while(play(game_data,matrix_game,choice,1) == -1){
                printf("The column selected is full, try again.\n");
                printf("Insert col [1,%i]: ",game_data->cols);
                scanf("%i",&choice);
            }
        else
            while(play(game_data,matrix_game,choice,2) == -1){
                printf("The column selected is full, try again.\n");
                printf("Insert col [1,%i]: ",game_data->cols);
                scanf("%i",&choice);
            }
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
