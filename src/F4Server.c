#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "errExit.h"

int rows, cols;
char p1_sign, p2_sign;

// function to check if player signs are valid
int chk_string_arg(char* s) {
    // sign must be a single character
    if (strlen(s) != 1)
        return 0;
    // sign cannot be |, -, _
    if (*s == '|' || *s == '_' || *s == '-')
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
            printf("Player signs must be single characters, excluded '|', '_' and '-'\n");
            return 1;
        }
    }

    return 0;
}
