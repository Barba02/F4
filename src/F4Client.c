#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "shared_memory.h"
#include "errExit.h"
#include "game.h"

int shmid,shmid_matrix;
game_t* game_data;
int **matrix_game;

// catches SIGUSR1
void sigUsr1Handler(int sig) {
    printf("Avversario trovato, è il turno.\n");
    // start game
    F4_game(game_data,matrix_game);
}

int main (int argc, char *argv[]) {
    int autoplay;
    char *username; // forse da mettere in memoria condivisa

    // setting SIGUSR1 handling
    if (signal(SIGUSR1, sigUsr1Handler) == SIG_ERR)
        errExit("Cannot change signal handler");

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

    //TODO: passare righe, colonne e segni utilizzati dai 2 giocatori
    //attach to game data
    shmid = alloc_shared_memory(sizeof(game_t), GAME_KEY);
    game_data = (game_t*) get_shared_memory(shmid);
    //attach to game matrix
    shmid_matrix = alloc_shared_memory(sizeof(int[game_data->rows][game_data->cols]),MATRIX_KEY);
    matrix_game = get_shared_memory(shmid_matrix);

    //check if there is already a user connected
    if(game_data->client1_pid == -1) {
        game_data->client1_pid = getpid();
    }
    else {
        game_data->client2_pid = getpid();
    }

    //notify server (check if is the first or second client)
    if(getpid() == game_data->client1_pid) {
        kill(game_data->server_pid, SIGUSR1);
        //first player wait for second player
        printf("In attesa di un avversario...\n");
        fflush(stdin);
    }
    else
        kill(game_data->server_pid,SIGUSR2);

    while (1);

    if (autoplay) {
        //TODO: gioco automatico con un bot
    }
    
    return 0;
}
