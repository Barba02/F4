#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "errExit.h"

int main (int argc, char *argv[]) {
    //matrix generation test
    int i, j;
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int matrix[rows][cols];
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 2;
        }
    }
    for (i = 0; i < rows+1; i++) {
        for (j = 0; j < cols+1; j++) 
        {
                if(j==cols)
                {
                    printf("|");
                }
                else
                    printf("| %i ", matrix[i][j]);
                    
        }
        printf("\n");
        for(int x=0; x < cols+1; x++)
        {
            printf("- - ");
        }
        printf("\n");
        
        
    }
    
    return 0;
}
