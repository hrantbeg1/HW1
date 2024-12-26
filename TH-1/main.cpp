#include <pthread.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>

struct ThreadData
{
    const std::vector<int>* arr;
    std::size_t start;
    std::size_t end;
    long long partial_sum;
};

void* calculate_partial_sum(void* arg)
{
    ThreadData* data = static_cast<ThreadData*>(arg);
    data->partial_sum = 0;
    for (std::size_t i = data->start; i < data->end; ++i)
    {
        data->partial_sum += (*data->arr)[i];
    }
    return nullptr;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <array_size> <thread_count>" << std::endl;
        return 1;
    }

    std::size_t N = std::stoull(argv[1]);
    int M = std::stoi(argv[2]);

    if (N < 1000000 || M <= 0)
    {
        std::cerr << "Error: Array size must be at least 1,000,000 and thread count must be positive." << std::endl;
        return 1;
    }

    std::vector<int> arr(N);
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = rand() % 100;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    long long single_thread_sum = 0;
    for (const auto& value : arr)
    {
        single_thread_sum += value;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> single_thread_duration = end_time - start_time;
    std::cout << "Time spent without threads: " << single_thread_duration.count() << " seconds" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    std::vector<pthread_t> threads(M);
    std::vector<ThreadData> thread_data(M);
    std::size_t chunk_size = N / M;

    for (int i = 0; i < M; ++i)
    {
        thread_data[i].arr = &arr;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i + 1) * chunk_size;
        if (i == M - 1 && thread_data[i].end < N)
        {
            thread_data[i].end = N;
        }
        pthread_create(&threads[i], nullptr, calculate_partial_sum, &thread_data[i]);
    }

    for (int i = 0; i < M; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> multi_thread_duration = end_time - start_time;
    std::cout << "Time spent with " << M << " threads: " << multi_thread_duration.count() << " seconds" << std::endl;

    return 0;
}
