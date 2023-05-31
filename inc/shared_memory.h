#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

typedef struct game{
    int rows;
    int cols;
    char p1_sign;
    char p2_sign;
    int **matrix_game;
} game_t;

int alloc_shared_memory(key_t shmKey, size_t size);

void *get_shared_memory(int shmid, int shmflg);

void free_shared_memory(void *ptr_sh);

void remove_shared_memory(int shmid);


#endif
