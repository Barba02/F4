#include <sys/shm.h>
#include <sys/stat.h>

#include "errExit.h"
#include "shared_memory.h"

int alloc_shared_memory(size_t size, key_t key) {
    int shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (shmid == -1)
        errExit("Cannot allocate shared memory segment");
    return shmid;
}

void *get_shared_memory(int shmid) {
    void *ptr = shmat(shmid, NULL, 0);
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
