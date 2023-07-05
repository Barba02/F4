#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "game.h"
#include "errExit.h"
#include "semaphores.h"

game_t* game_data; // shared struct containing game data
int (*game_matrix)[]; // shared game matrix
int semid; // semaphore set to guarantee mutex and playing alternation

// setting terminal behaviour to not print ^C and restore at the end
struct termios save;
void reset_terminal() {
    tcsetattr(0, 0, &save);
}
void clear_terminal() {
    tcgetattr(0, &save);
    struct termios new = save;
    new.c_lflag &= ~ECHOCTL;
    tcsetattr(0, 0, &new);
    atexit(reset_terminal);
}

// functions to deattach shared memory segments on exit
void deattach_shmid_data() {
    free_shared_memory(game_data);
}
void deattach_shmid_matrix() {
    free_shared_memory(game_matrix);
}

// catches SIGUSR1 (start game after other player arrived)
void sigUsr1Handler(int sig) {
    printf("You are playing against %s\n", game_data->client2_username);
    F4_game(game_data, game_matrix, semid);
}

// catches SIGTERM and manage closing
void sigTermHandler(int sig) {
    if (game_data->server_terminate) {
        // server termination game ended
        if (game_data->win != 0) {
            if ((game_data->last_player == 1 && getpid() == game_data->client2_pid) ||
                (game_data->last_player == 2 && getpid() == game_data->client1_pid))
                print_game(game_data->rows, game_data->cols, game_matrix, game_data->client1_sign, game_data->client2_sign);
            if (game_data->win == 1) {
                if (game_data->last_player == 1) {
                    if (getpid() == game_data->client1_pid)
                        printf("\nYou won\n");
                    else
                        printf("\nYou lost\n");
                }
                else {
                    if (getpid() == game_data->client2_pid)
                        printf("\nYou won\n");
                    else
                        printf("\nYou lost\n");
                }
            }
            else
                printf("\nFull table, game ended\n");
        }
        // server termination double CTRL+C
        else
            printf("\nServer terminated the game, nobody won\n");
    }
    // other client quit
    else {
        if (game_data->client1_pid == -1)
            printf("\n\n%s quit, you won\n", game_data->client1_username);
        else if (game_data->client2_pid == -1)
            printf("\n\n%s quit, you won\n", game_data->client2_username);
    }
    exit(0);
}

// catches SIGINT and manage closing
void sigIntHandler(int sig) {
    // first client quit
    if (getpid() == game_data->client1_pid) {
        game_data->client1_pid = -1;
        kill(game_data->server_pid, SIGUSR1);
    }
    // second client quit
    else {
        game_data->client2_pid = -1;
        kill(game_data->server_pid, SIGUSR2);
    }
    // close the process
    printf("\n\nYou quit and lost the game\n");
    exit(0);
}

int main(int argc, char *argv[]) {

    // setting SIGINT handling
    clear_terminal();
    if (signal(SIGINT, sigIntHandler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGTERM handling
    if (signal(SIGTERM, sigTermHandler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGUSR1 handling
    if (signal(SIGUSR1, sigUsr1Handler) == SIG_ERR)
        errExit("Cannot change signal handler");

    // attaching to game data shared memory
    if ((game_data = (game_t*) retrieve_shm_segment(sizeof(game_t), GAME_KEY)) == NULL) {
        printf("Server not running\n");
        exit(1);
    }
    atexit(deattach_shmid_data);

    // check command line arguments number
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <username> [autoPlay(1)]\n", argv[0]);
        return 1;
    }
    // validation of the arguments
    else {
        // check if it's the bot playing
        if (!game_data->autoplay)
            // check if the client is requested to be a bot
            game_data->autoplay = (argc == 3 && strcmp(argv[2], "1") == 0) ? 1 : 0;
    }

    // attach to game matrix
    if ((game_matrix = retrieve_shm_segment(sizeof(int[game_data->rows][game_data->cols]), MATRIX_KEY)) == NULL) {
        printf("Server not running\n");
        exit(1);
    }
    atexit(deattach_shmid_matrix);

    // get semaphore set
    if ((semid = semget(SEM_KEY,3,S_IRUSR | S_IWUSR))== -1)
        errExit("Cannot get semaphores");

    //check if two clients are already connected
    if(game_data->client1_pid != -1 && game_data->client2_pid != -1)
    {
        printf("Cannot connect to the game: Two players are already connected \n");
        exit(0);
    }
    // check if this client is user 1 or 2
    if (game_data->client1_pid == -1) {
        game_data->client1_pid = getpid();
        strcpy(game_data->client1_username, argv[1]);
    }
    else {
        game_data->client2_pid = getpid();
        strcpy(game_data->client2_username, argv[1]);
    }

    // first client notify server and wait for the second
    if (getpid() == game_data->client1_pid) {
        kill(game_data->server_pid, SIGUSR1);
        printf("Your sign is %c\n\n", game_data->client1_sign);
        printf("Waiting for another player...\n");
        pause();
    }
    // second client notify server and wait game start
    else {
        printf("Your sign is %c\n\n", game_data->client2_sign);
        printf("Waiting for %s to start the game...\n", game_data->client1_username);
        kill(game_data->server_pid, SIGUSR2);
        F4_game(game_data, game_matrix, semid);
    }

    return 0;
}
