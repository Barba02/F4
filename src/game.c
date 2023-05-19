#include "../inc/game.h"
#include <stdio.h>

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
