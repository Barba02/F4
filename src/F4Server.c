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
#include "shared_memory.h"
#include "errExit.h"

#define matrix_game_key 67890

int rows, cols, catcher = 0; // TODO: reset counter a ogni input
char p1_sign = ' ', p2_sign = ' ';

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
        // TODO: chiusura memoria e semafori
        exit(0);
    }
    else
        if (signal(SIGINT, sigIntHandler) == SIG_ERR)
            errExit("Cannot change signal handler");
}

// signs assignment function
char random_char() {
    // if X has already given to p1 or chosen by the user, returns O
    if (toupper(p1_sign) == 'X')
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
    rows = (int) strtol(args[1], &ptr, 10);
    if (strlen(ptr) != 0 || rows < 5)
        return 1;
    // check that columns is a number >=5
    cols = (int) strtol(args[2], &ptr, 10);
    if (strlen(ptr) != 0 || cols < 5)
        return 1;
    // check optional player1 sign
    if (n >= 4) {
        if (!chk_string_arg(args[3]))
            return 2;
        p1_sign = *args[3];
    }
    // check optional player2 sign
    if (n == 5) {
        if (!chk_string_arg(args[4]))
            return 2;
        p2_sign = *args[4];
    }
    // arguments are ok
    return 0;
}

int main (int argc, char *argv[]) {
    // setting SIGINT handling
    clear_terminal();
    if (signal(SIGINT, sigIntHandler) == SIG_ERR)
        errExit("Cannot change signal handler");

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

    // player signs assignment
    if (p1_sign == ' ')
        p1_sign = random_char();
    if (p2_sign == ' ')
        p2_sign = random_char();

    //initialize shared memory for matrix game
    int shmid;
    size_t size = sizeof(int[rows][cols]);
    int (*matrix_game)[cols];
    //TODO: modificare funzione di allocazione per il server (deve dare errore se già allocata)
    shmid = alloc_shared_memory(matrix_game_key,size);
    matrix_game = get_shared_memory(shmid,0);


    return 0;
}
