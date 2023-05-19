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

int main (int argc, char *argv[]) {
    //TODO: passare righe, colonne e segni utilizzati dai 2 giocatori
    //attach to matrix game
    int shmid;
    size_t size = sizeof(int[5][5]);
    int (*matrix_game)[5];
    shmid = alloc_shared_memory(matrix_game_key,size);
    matrix_game = get_shared_memory(shmid,0);

    
    return 0;
}
