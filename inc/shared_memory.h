#ifndef _SHARED_MEMORY_HH
#define _SHARED_MEMORY_HH

#include <stdlib.h>

#define GAME_KEY 67890
#define MATRIX_KEY 54321

typedef struct game {
    pid_t server_pid;
    pid_t client_pid[2];
    char client_sign[2];
    char client_username[2][64];
    int win;
    int rows;
    int cols;
    int autoplay;
    int last_player;
    int server_terminate;
} game_t;

int alloc_shared_memory(size_t size, key_t key, int ipc_excl);

void *get_shared_memory(int shmid);

void free_shared_memory(void *ptr_sh);

void remove_shared_memory(int shmid);

int shm_already_existent(size_t size, key_t key);

void *retrieve_shm_segment(size_t size, key_t key);

#endif
