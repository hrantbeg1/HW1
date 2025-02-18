#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 5
#define SHM_NAME "/print_lock"

sem_t chopsticks[N];
pid_t philosophers[N];
sem_t *print_lock;
volatile sig_atomic_t stop = 0;

void handle_interrupt() { stop = 1; }
void handle_terminate() { exit(0); }

void log_event(const char *msg, int id) {
    sem_wait(print_lock);
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[9];
    strftime(time_str, 9, "%H:%M:%S", tm_info);
    printf("[%s] Philosopher %d: %s\n", time_str, id, msg);
    fflush(stdout);
    sem_post(print_lock);
}

void philosopher(int id) {
    signal(SIGTERM, handle_terminate);
    srand(time(NULL) ^ getpid());

    int left = id;
    int right = (id + 1) % N;

    if (id % 2 == 0) { 
        int temp = left;
        left = right;
        right = temp;
    }

    while (!stop) {
        log_event("Thinking...", id);
        sleep(rand() % 3 + 1);

        if (sem_wait(&chopsticks[left]) == 0) {
            if (sem_wait(&chopsticks[right]) == 0) {
                log_event("Eating...", id);
                sleep(rand() % 3 + 1);
                sem_post(&chopsticks[right]);
            }
            sem_post(&chopsticks[left]);
        }
    }
    exit(0);
}

void cleanup() {
    printf("\nCleaning up resources...\n");
    for (int i = 0; i < N; ++i) {
        sem_destroy(&chopsticks[i]);
    }
    sem_destroy(print_lock);
    munmap(print_lock, sizeof(sem_t));
    shm_unlink(SHM_NAME);
    printf("Cleanup completed.\n");
}

int main() {
    signal(SIGINT, handle_interrupt);
    signal(SIGTERM, cleanup);
    signal(SIGSEGV, cleanup);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(sem_t));
    print_lock = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    sem_init(print_lock, 1, 1);

    for (int i = 0; i < N; ++i) {
        if (sem_init(&chopsticks[i], 1, 1) == -1) {
            perror("sem_init");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < N; ++i) {
        philosophers[i] = fork();
        if (philosophers[i] == -1) {
            perror("Error creating process");
            for (int j = 0; j < i; ++j) {
                kill(philosophers[j], SIGTERM);
                waitpid(philosophers[j], NULL, 0);
            }
            cleanup();
            exit(EXIT_FAILURE);
        }
        if (philosophers[i] == 0) {
            philosopher(i);
        }
    }

    while (!stop) {
        sleep(1);
    }

    for (int i = 0; i < N; ++i) {
        kill(philosophers[i], SIGTERM);
        waitpid(philosophers[i], NULL, 0);
    }

    cleanup();
    return 0;
}
