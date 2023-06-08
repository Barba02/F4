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

int shmid_data; // shared segment's id for game data
game_t* game_data; // shared struct containing game data
int shmid_matrix; // shared segment's id for game matrix
int (*game_matrix)[]; // shared matrix to play

// catches SIGUSR1
void sigUsr1Handler(int sig) {
    printf("Other player found\n");
    // start game
    F4_game(game_data, game_matrix);
}

int main (int argc, char *argv[]) {
    int autoplay;

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
        autoplay = (argc == 3 && strcmp(argv[2], "1") == 0) ? 1 : 0;
    }

    // attach to game data
    shmid_data = alloc_shared_memory(sizeof(game_t), GAME_KEY);
    game_data = (game_t*) get_shared_memory(shmid_data);

    // attach to game matrix
    shmid_matrix = alloc_shared_memory(sizeof(int[game_data->rows][game_data->cols]),MATRIX_KEY);
    game_matrix = get_shared_memory(shmid_matrix);

    // check if this client is user 1 or 2
    if (game_data->client1_pid == -1) {
        game_data->client1_pid = getpid();
        strcpy(game_data->client1_username,argv[1]);
    }
    else {
        game_data->client2_pid = getpid();
        strcpy(game_data->client2_username,argv[1]);
    }

    //notify server (check if is the first or second client)
    if (getpid() == game_data->client1_pid) {
        kill(game_data->server_pid, SIGUSR1);
        //first player wait for second player
        printf("Waiting for another player...\n");
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
