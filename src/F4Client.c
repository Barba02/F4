#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "shared_memory.h"
#include "errExit.h"

#define matrix_game_key 67890

int main (int argc, char *argv[]) {
    int autoplay;
    char *username; // forse da mettere in memoria condivisa

    // check command line arguments number
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <username> [autoPlay]\n", argv[0]);
        return 1;
    }
    // validation of the arguments
    else {
        username = argv[1];
        autoplay = (argc == 3 && strcmp(argv[2], "1") == 0) ? 1 : 0;
    }

    if (autoplay) {
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
