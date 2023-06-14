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
#include <sys/stat.h>
#include "errExit.h"
#include "shared_memory.h"
#include "game.h"

int catcher = 0; // counter to kill the process
int shmid_data; // shared segment's id for game data
game_t* game_data; // shared struct containing game data
int shmid_matrix; // shared segment's id for game matrix
// int (*game_matrix)[]; // shared matrix to play

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

// catches SIGINT and manage closing
void sigIntHandler(int sig) {
    if (++catcher == 2) {
        // TODO: chiusura semafori

        // terminate connected clients
        if(game_data->client1_pid != -1 && game_data->client2_pid != -1){
            kill(game_data->client1_pid,SIGTERM);
            kill(game_data->client2_pid,SIGTERM);
        }

        free_shared_memory(game_data);
        remove_shared_memory(shmid_data);
        // free_shared_memory(game_matrix);
        remove_shared_memory(shmid_matrix);
        exit(0);
    }
    printf("Press CTRL+C another time to quit\n");
}

// catches SIGUSR1
void sigUsr1Handler(int sig) {
    printf("%s (%c) is ready to play\n", game_data->client1_username, game_data->client1_sign);
}

// catches SIGUSR2
void sigUsr2Handler(int sig) {
    printf("%s (%c) is ready to play\n", game_data->client2_username, game_data->client2_sign);
    kill(game_data->client1_pid,SIGUSR1);
}

// signs assignment function
char random_char() {
    // if X has already given to p1 or chosen by the user, returns O
    if (toupper(game_data->client1_sign) == 'X')
        return 'O';
    // return X in the other cases
    return 'X';
}

// function to check if player signs are valid
int chk_string_arg(char* s) {
    // sign must be a single character
    if (strlen(s) != 1)
        return 0;
    // sign cannot be '|', '-', '_', ' '
    if (*s == '|' || *s == '_' || *s == '-' || *s == ' ')
        return 0;
    // sign is ok
    return 1;
}

// function to check if the arguments are valid
int chk_args(int n, char** args) {
    char *ptr;
    // check that rows is a number >=5
    game_data->rows = (int) strtol(args[1], &ptr, 10);
    if (strlen(ptr) != 0 || game_data->rows < 5)
        return 1;
    // check that columns is a number >=5
    game_data->cols = (int) strtol(args[2], &ptr, 10);
    if (strlen(ptr) != 0 || game_data->cols < 5)
        return 1;
    // check optional player1 sign
    if (n >= 4) {
        if (!chk_string_arg(args[3]))
            return 2;
        game_data->client1_sign = *args[3];
    }
    // check optional player2 sign
    if (n == 5) {
        if (!chk_string_arg(args[4]))
            return 2;
        game_data->client2_sign = *args[4];
    }
    // arguments are ok
    return 0;
}

int main (int argc, char *argv[]) {
    // setting SIGINT handling
    clear_terminal();
    if (signal(SIGINT, sigIntHandler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGUSR1 handling
    if (signal(SIGUSR1, sigUsr1Handler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGUSR2 handling
    if (signal(SIGUSR2, sigUsr2Handler) == SIG_ERR)
        errExit("Cannot change signal handler");

    //TODO: inizializzare semafori

    // initialize shared memory for game data
    shmid_data = alloc_shared_memory(sizeof(game_t), GAME_KEY);
    game_data = (game_t*) get_shared_memory(shmid_data);

    // check command line arguments number
	if (argc < 3 || argc > 5) {
        printf("Usage: %s <rows> <columns> [player1 sign] [player2 sign]\n", argv[0]);
        return 1;
    }
    // validation of the arguments
    else {
        switch (chk_args(argc, argv)) {
        case 1:
            printf("Rows and columns arguments must be numbers greater or equal to 5\n");
            return 1;
        case 2:
            printf("Player signs must be single characters, excluded '|', '-', '_', ' '\n");
            return 1;
        }
    }

    //initzialize play counter
    game_data->n_played=0;

    // player signs assignment
    if (game_data->client1_sign == '\0')
        game_data->client1_sign = random_char();
    if (game_data->client2_sign == '\0')
        game_data->client2_sign = random_char();

    // initialize pids in shared struct
    game_data->server_pid = getpid();
    game_data->client1_pid = -1;
    game_data->client2_pid = -1;

    // initialize shared memory for game matrix
    int (*game_matrix)[game_data->cols]; // TODO: deve essere globale
    shmid_matrix = alloc_shared_memory(sizeof(int[game_data->rows][game_data->cols]),MATRIX_KEY);
    game_matrix = get_shared_memory(shmid_matrix);

    // continue until matrix are full or one player win
    while (game_data->n_played<game_data->rows*game_data->cols && !check_win(game_data,game_matrix));

    // terminate connected clients
    kill(game_data->client1_pid,SIGTERM);
    kill(game_data->client2_pid,SIGTERM);

    // TODO: chiusura semafori
    free_shared_memory(game_data);
    remove_shared_memory(shmid_data);
    // free_shared_memory(game_matrix);
    remove_shared_memory(shmid_matrix);

    return 0;
}
