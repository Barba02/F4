#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

#define GAME_KEY 67890

typedef struct game {
    int rows;
    int cols;
    char p1_sign;
    char p2_sign;
    int **matrix_game;
} game_t;

int alloc_shared_memory(size_t size);

void *get_shared_memory(int shmid);

void free_shared_memory(void *ptr_sh);

void remove_shared_memory(int shmid);


#endif
