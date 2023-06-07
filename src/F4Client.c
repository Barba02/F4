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

int shmid;
game_t* game_table;

// catches SIGUSR1
void sigUsr1Handler(int sig) {
    printf("Avversario trovato, è il turno.\n");
    //TODO: iniziare il gioco
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
    //attach to matrix game
    shmid = alloc_shared_memory(sizeof(game_t));
    game_table = (game_t*) get_shared_memory(shmid);

    //check if there is already a user connected
    if(game_table->client1_pid == -1){
        game_table->client1_pid=getpid();
    }
    else{
        game_table->client2_pid=getpid();
    }

    //notify server (check if is the first or second client)
    if(getpid() == game_table->client1_pid) {
        kill(game_table->server_pid, SIGUSR1);
        //first player wait for second player
        printf("In attesa di un avversario...\n");
        fflush(stdin);
    }
    else
        kill(game_table->server_pid,SIGUSR2);


    //print_game(game_table->rows,game_table->cols,game_table->matrix_game,game_table->p1_sign,game_table->p2_sign);

    if (autoplay) {
        //TODO: gioco automatico con un bot
    }
    
    return 0;
}
