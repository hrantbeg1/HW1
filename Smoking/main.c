#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define EMPTY '-'
#define TOBACCO 0
#define PAPER 1
#define MATCHES 2
#define WAIT_TIME 1

typedef struct s_bar {
    char resources[100];
    pid_t smoker_pids[4];
    sem_t item_sems[3];
    sem_t bartender_sem;
    size_t stock[3];
    int smokers_done;
} t_bar;

void evict_all(t_bar *bar) {
    printf("Closing the bar, smokers must leave\n");
    for (int i = 0; i < 3; ++i) {
        kill(bar->smoker_pids[i], SIGTERM);
        waitpid(bar->smoker_pids[i], NULL, 0);
    }
    exit(EXIT_SUCCESS);
}

void bartender(t_bar *bar) {
    for (int i = 0; i < 3; ++i) {
        if (bar->stock[i] == 0)
            bar->smokers_done++; }
    size_t idx = 0;
    while (1) {
        if (bar->smokers_done == 3) {
            evict_all(bar);
            exit(EXIT_SUCCESS);}
        while (idx < strlen(bar->resources)) {
            sem_wait(&bar->bartender_sem);
            printf("\nBartender looks for resources\n");
            sleep(WAIT_TIME);
            if (bar->resources[idx] == 't') {
                printf("Bartender gives tobacco\n");
                sem_post(&bar->item_sems[TOBACCO]);
    } else if (bar->resources[idx] == 'p') {
                printf("Bartender gives paper\n");
                sem_post(&bar->item_sems[PAPER]);
            } else if (bar->resources[idx] == 'm') {
                printf("Bartender gives matches\n");
                sem_post(&bar->item_sems[MATCHES]);
            } else {
                sem_post(&bar->bartender_sem);
            }
            bar->resources[idx] = EMPTY;
            idx++;
        }
    }
}

void smoker(t_bar *bar, int id) {
    while (1) {
        sem_wait(&bar->item_sems[id]);
        if (bar->stock[id] > 0) {
            --bar->stock[id];
            printf("Smoker %d received needed item\n", id);
        }
        sleep(WAIT_TIME);
        sem_post(&bar->bartender_sem);
        if (bar->stock[id] == 0) {
            printf("Smoker %d finished smoking and left\n", id);
            bar->smokers_done++;
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {
    t_bar *bar = mmap(NULL, sizeof(t_bar), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (bar == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    if (scanf("%s", bar->resources) == EOF) {
        perror("input error");
        munmap(bar, sizeof(t_bar));
        exit(EXIT_FAILURE);
    }

    if (!strchr(bar->resources, 't') && !strchr(bar->resources, 'p') &&
        !strchr(bar->resources, 'm')) {
        printf("Invalid input\n");
        munmap(bar, sizeof(t_bar));
        exit(EXIT_FAILURE);
    }

    bar->smokers_done = 0;
    bar->stock[TOBACCO] = 0;
    bar->stock[PAPER] = 0;
    bar->stock[MATCHES] = 0;
    for (size_t i = 0; i < strlen(bar->resources); ++i) {
        if (bar->resources[i] == 't')
            ++bar->stock[TOBACCO];
        else if (bar->resources[i] == 'p')
            ++bar->stock[PAPER];
        else if (bar->resources[i] == 'm')
            ++bar->stock[MATCHES];
    }

    for (int i = 0; i < 3; ++i)
        sem_init(&bar->item_sems[i], 1, 0);
    sem_init(&bar->bartender_sem, 1, 1);

    srand(time(NULL));
    for (int i = 0; i < 4; ++i) {
        bar->smoker_pids[i] = fork();
        if (bar->smoker_pids[i] < 0) {
            perror("fork error");
            for (int j = i - 1; j >= 0; --j)
                kill(bar->smoker_pids[j], SIGTERM);
            munmap(bar, sizeof(t_bar));
            exit(errno);
        }
        if (bar->smoker_pids[i] == 0) {
            if (i == 3)
                bartender(bar);
            else
                smoker(bar, i);
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < 4; ++i)
        wait(NULL);

    for (int i = 0; i < 3; ++i)
        sem_destroy(&bar->item_sems[i]);
    sem_destroy(&bar->bartender_sem);

    munmap(bar, sizeof(t_bar));
}
