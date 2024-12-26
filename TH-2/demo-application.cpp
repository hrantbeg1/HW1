#include "parallel_scheduler.h"
#include <iostream>
#include <chrono>
#include <thread>

void task_1()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task 1 completed" << std::endl;
}

void task_2()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Task 2 completed" << std::endl;
}

void task_3()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Task 3 completed" << std::endl;
}

int main()
{
    parallel_scheduler scheduler(2);

    scheduler.run(task_1);
    scheduler.run(task_2);
    scheduler.run(task_3);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
