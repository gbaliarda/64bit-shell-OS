#ifndef SHM_H
#define SHM_H

void shmInit();

void *shmAttach(int id);

void shmClose(int id);

#endif