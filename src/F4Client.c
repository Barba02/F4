#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <termios.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "game.h"
#include "errExit.h"
#include "semaphores.h"
#include "shared_memory.h"

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

// catches SIGTERM
void sigTermHandler(int sig) {
    printf("\nGAME OVER : ");
    if(game_data->n_played == game_data->rows*game_data->cols)
        printf("DRAW\n");
    else
        printf("%s WIN!\n",(game_data->last_player == 1)? game_data->client1_username : game_data->client2_username);
    exit(0);
}

int main (int argc, char *argv[]) {
    // setting SIGUSR1 handling
    if (signal(SIGUSR1, sigUsr1Handler) == SIG_ERR)
        errExit("Cannot change signal handler");

    // setting SIGUSR1 handling
    if (signal(SIGTERM, sigTermHandler) == SIG_ERR)
        errExit("Cannot change signal handler");

    // attach to game data
    shmid_data = alloc_shared_memory(sizeof(game_t), GAME_KEY, 0);
    game_data = (game_t*) get_shared_memory(shmid_data);

    // check command line arguments number
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <username> [autoPlay]\n", argv[0]);
        return 1;
    }
    // validation of the arguments
    else {
        //Se è gia stato settato a 1 sono il bot e non faccio niente, altrimenti lo setto
        if(!game_data->autoplay)
            game_data->autoplay = (argc == 3 && strcmp(argv[2], "1") == 0) ? 1 : 0;
    }

    // get shared memory acccess semaphores
    /* int shm_access_semid = semget(SHM_ACCESS, 2, S_IRUSR | S_IWUSR);
    if (shm_access_semid == -1)
        errExit("Cannot get semaphores"); */

    // attach to game data
    // semOp(shm_access_semid, 0, -1);
    shmid_data = alloc_shared_memory(sizeof(game_t), GAME_KEY, 0);
    game_data = (game_t*) get_shared_memory(shmid_data);

    // attach to game matrix
    // semOp(shm_access_semid, 1, -1);
    shmid_matrix = alloc_shared_memory(sizeof(int[game_data->rows][game_data->cols]),MATRIX_KEY, 0);
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
    
    while(1);

    return 0;
}
