#include "parallel_scheduler.h"
#include <iostream>

parallel_scheduler::parallel_scheduler(size_t capacity)
    : capacity_(capacity), stopped(false)
{
    threads = new pthread_t[capacity_];
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);

    for (size_t i = 0; i < capacity_; ++i)
    {
        pthread_create(&threads[i], nullptr, &parallel_scheduler::thread_worker, this);
    }
}

parallel_scheduler::~parallel_scheduler()
{
    stop();
    delete[] threads;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void parallel_scheduler::run(std::function<void()> task)
{
    pthread_mutex_lock(&mutex);
    tasks.push(task);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void parallel_scheduler::stop()
{
    pthread_mutex_lock(&mutex);
    stopped = true;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for (size_t i = 0; i < capacity_; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void* parallel_scheduler::thread_worker(void* arg)
{
    parallel_scheduler* scheduler = static_cast<parallel_scheduler*>(arg);
    scheduler->worker_thread();
    return nullptr;
}

void parallel_scheduler::worker_thread()
{
    while (true)
    {
        pthread_mutex_lock(&mutex);

        while (tasks.empty() && !stopped)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        if (stopped && tasks.empty())
        {
            pthread_mutex_unlock(&mutex);
            return;
        }

        auto task = tasks.front();
        tasks.pop();
        pthread_mutex_unlock(&mutex);

        task();
    }
}
