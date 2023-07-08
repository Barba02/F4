/************************************
*VR472408,VR471509,VR446245
*Barbieri Filippo,Brighenti Alessio,Taouri Islam
*07/07/2023
*************************************/

#include "errExit.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void errExit(const char *msg) {
    // print error message passed as argument
    perror(msg);
    // exit the program with bad exit
    exit(1);
}
