#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

#define GAME_KEY 67890
#define MATRIX_KEY 54321

typedef struct game {
    pid_t server_pid;
    pid_t client1_pid;
    pid_t client2_pid;
    int last_player;
    int rows;
    int cols;
    char client1_sign;
    char client2_sign;
    char client1_username[25];
    char client2_username[25];
    int n_played;
} game_t;

int alloc_shared_memory(size_t size, key_t key);

void *get_shared_memory(int shmid);

void free_shared_memory(void *ptr_sh);

void remove_shared_memory(int shmid);

#endif
