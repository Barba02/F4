#include <sys/sem.h>
#include <sys/stat.h>

#include "errExit.h"
#include "semaphores.h"

void semOp (int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop = {sem_num, sem_op, 0};
    if (semop(semid, &sop, 1) == -1)
        errExit("semop failed");
}

int create_sem_set(key_t semkey, int nsems, unsigned short *values) {
    int semid = semget(semkey, nsems, IPC_CREAT | S_IRUSR | S_IWUSR | IPC_EXCL);
    if (semid == -1)
        errExit("semget failed");
    union semun arg;
    arg.array = values;
    if (semctl(semid, 0, SETALL, arg) == -1)
        errExit("semctl SETALL failed");
    return semid;
}

void remove_sem_set(int semid) {
    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        errExit("Cannot delete sempahores");
}
