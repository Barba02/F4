#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "../inc/shared_memory.h"
#include "../inc/errExit.h"
#include "../inc/game.h"

#define matrix_game_key 67890

// function to check if player signs are valid
int chk_string_arg(char* s) {
    // flag must be "-auto"
    if (strcmp(s,"-auto") != 0)
        return 0;
    // sign is ok
    return 1;
}

int main (int argc, char *argv[]) {

    // check command line arguments number
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <username> [flag] \n", argv[0]);
        return 1;
    }

    if(argc == 3){
        if(!chk_string_arg(argv[2])){
            printf("Usage: %s <username> -auto \n", argv[0]);
            return 1;
        }
        //TODO: gioco automatico con un bot
    }

    //TODO: passare righe, colonne e segni utilizzati dai 2 giocatori
    //attach to matrix game
    int shmid;
    size_t size = sizeof(int[5][5]);
    int (*matrix_game)[5];
    shmid = alloc_shared_memory(matrix_game_key,size);
    matrix_game = get_shared_memory(shmid,0);

    
    return 0;
}
