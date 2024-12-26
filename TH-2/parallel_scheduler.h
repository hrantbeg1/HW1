#ifndef PARALLEL_SCHEDULER_H
#define PARALLEL_SCHEDULER_H

#include <pthread.h>
#include <cstddef>
#include <functional>
#include <queue>

class parallel_scheduler
{
private:
    size_t capacity_;
    pthread_t* threads;
    std::queue<std::function<void()>> tasks;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool stopped;

    static void* thread_worker(void* arg);
    void worker_thread();

public:
    parallel_scheduler(size_t capacity);
    ~parallel_scheduler();
    void run(std::function<void()> task);
    void stop();
};

#endif
