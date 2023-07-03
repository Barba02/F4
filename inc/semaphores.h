#ifndef _SEMAPHORES_HH
#define _SEMAPHORES_HH

#include <stdlib.h>

#define SEM_KEY 10928

union semun {
    int val;
    struct semi_ds *buf;
    unsigned short *array;
};

void semOp(int semid, unsigned short sem_num, short sem_op);

int create_sem_set(key_t semkey, int nsems, unsigned short *values);

void remove_sem_set(int semid);

#endif
