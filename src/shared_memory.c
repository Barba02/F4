#include <sys/shm.h>
#include <sys/stat.h>

#include "../inc/errExit.h"
#include "../inc/shared_memory.h"

int alloc_shared_memory(key_t shmKey, size_t size) {
    int shmid = shmget(shmKey, size, IPC_CREAT | S_IRWXU);
    if (shmid == -1)
        errExit("Cannot allocate shared memory segment");
    return shmid;
}

void *get_shared_memory(int shmid, int shmflg) {
    void *ptr = shmat(shmid, NULL, shmflg);
    if (ptr == (void *)-1)
        errExit("shmat failed");
    return ptr;
}

void free_shared_memory(void *ptr) {
    if (shmdt(ptr) == -1)
        errExit("Cannot detach shared memory segment");
}

void remove_shared_memory(int shmid) {
    // delete the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        errExit("Cannot delete shared memory segment");
}
