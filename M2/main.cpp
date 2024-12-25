#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
sem_t* sem;
int* tiv;

int main(int ac, char** av) {
    if (ac < 2 || ac > 2) {
        std::cerr << std::endl;
        exit(EXIT_FAILURE);
    }

void plus() {
    for (int i = 1; i <= 100000; ++i) {
        sem_wait(sem);
        (*tiv)++;
        sem_post(sem);
    }
}

int shm_fd = shm_open("shared", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (shm_fd < 0) {
	perror("shmopen error");
        exit(EXIT_FAILURE);
    }

 if (ftruncate(shm_fd, 4) < 0) {
	perror("error truncate");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

void* post = mmap(nullptr, 4, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if (post == MAP_FAILED) {
	perror("mmap error");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

tiv = static_cast<int*>(post);

*tiv = 25;

sem = sem_open("sem", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
	perror("error semopen");
        munmap(post, 4);
        close(shm_fd);
	shm_unlink("sharing");
        exit(EXIT_FAILURE);
    }

int A = atoi(av[1]);
    for (int i = 0; i < A; ++i) {
        int pd = fork();
        if (pd < 0) {
	    perror("fork error");
            sem_close(sem);
	    sem_unlink("sem");
            munmap(post, 4);
            close(shm_fd);
	    shm_unlink("sharing");
            exit(EXIT_FAILURE);
        }

        if (pd == 0) {
            plus();
            exit(0);
        }
    }

for (int i = 0; i < A; ++i) {
        wait(nullptr);
    }

    std::cout << *tiv << std::endl;

    if (munmap(post, 4) < 0) {
        perror("map  error ");
    }
    close(shm_fd);
    shm_unlink("sharing ");
    sem_close(sem);
    sem_unlink("sem");

    return 0;
}










