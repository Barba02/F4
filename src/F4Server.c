#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include "game.h"
#include "errExit.h"
#include "semaphores.h"

int catcher = 0; // CTRL+C counter to kill the process
int shmid_data; // game data shared segment id
game_t* game_data; // shared struct containing game data
int shmid_matrix; // game matrix shared segment id
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
// TODO: vincita in caso di arresa di un client

// functions to close shared memory segments and semaphores on exit
void close_shmid_data() {
    free_shared_memory(game_data);
    remove_shared_memory(shmid_data);
}
void close_shmid_matrix() {
    free_shared_memory(game_matrix);
    remove_shared_memory(shmid_matrix);
}
void close_semid() {
    remove_sem_set(semid);
}

// catches SIGTERM and manage closing
void sigTermHandler(int sig) {
    printf("Client quit\n");
    exit(0);
}

// catches SIGINT and manage closing
void sigIntHandler(int sig) {
    if (++catcher == 2) {
        // terminate connected clients
        game_data->server_terminate = 1;
        if (game_data->client1_pid != -1)
            kill(game_data->client1_pid, SIGTERM);
        if (game_data->client2_pid != -1)
            kill(game_data->client2_pid, SIGTERM);
        exit(0);
    }
    printf("Press CTRL+C another time to quit\n");
}

// catches SIGUSR1 (first client connected)
void sigUsr1Handler(int sig) {
    printf("%s (%c) is ready to play\n", game_data->client1_username, game_data->client1_sign);
    if (game_data->autoplay) {
        pid_t autoPid = fork();
        // close client if child process cannot be created
        if (autoPid == -1) {
            game_data->server_terminate = 1;
            kill(game_data->client1_pid, SIGTERM);
        }
        else if (autoPid == 0) {
            if (execl("./F4Client", "./F4Client", "bot", NULL) == -1)
                errExit("Error exec autoplay");
        }
    }
}

// catches SIGUSR2 (second client connected)
void sigUsr2Handler(int sig) {
    printf("%s (%c) is ready to play\n", game_data->client2_username, game_data->client2_sign);
    // alert first client
    kill(game_data->client1_pid, SIGUSR1);
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
    // setting SIGTERM handling
    if (signal(SIGTERM, sigTermHandler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGUSR1 handling
    if (signal(SIGUSR1, sigUsr1Handler) == SIG_ERR)
        errExit("Cannot change signal handler");
    // setting SIGUSR2 handling
    if (signal(SIGUSR2, sigUsr2Handler) == SIG_ERR)
        errExit("Cannot change signal handler");

    // initialize game data shared memory
    shmid_data = alloc_shared_memory(sizeof(game_t), GAME_KEY, 1);
    game_data = (game_t*) get_shared_memory(shmid_data);
    atexit(close_shmid_data);

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

    // initialize game variables
    game_data->autoplay = 0;
    game_data->n_played = 0;
    game_data->server_terminate = 0;

    // player signs assignment
    if (game_data->client1_sign == '\0')
        game_data->client1_sign = random_char();
    if (game_data->client2_sign == '\0')
        game_data->client2_sign = random_char();

    // initialize pids in shared struct
    game_data->server_pid = getpid();
    game_data->client1_pid = -1;
    game_data->client2_pid = -1;

    // initialize semaphore set
    unsigned short sem_init_values[]={0,1,0};   // [0]:server [1]:player1 turn [2]:player2 turn
    semid = create_sem_set(SEM_KEY,3,sem_init_values);
    atexit(close_semid);

    // initialize game matrix shared memory
    shmid_matrix = alloc_shared_memory(sizeof(int[game_data->rows][game_data->cols]), MATRIX_KEY, 1);
    game_matrix = get_shared_memory(shmid_matrix);
    atexit(close_shmid_matrix);

    // waiting SIGUSR1
    pause();
    // waiting SIGUSR2
    pause();

    while (1) {
        semOp(semid,0,-1);
        // check win or full matrix
        if (game_data->n_played==game_data->rows*game_data->cols || check_win(game_data->rows, game_data->cols, game_matrix)){
            // terminate connected clients
            game_data->server_terminate = 1;
            kill(game_data->client1_pid,SIGTERM);
            kill(game_data->client2_pid,SIGTERM);
            break;
        }
        else {
            if (game_data->last_player == 1)
                semOp(semid,2,1);
            else
                semOp(semid,1,1);
        }
    }

    return 0;
}
