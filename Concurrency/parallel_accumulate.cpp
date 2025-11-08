#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ostream>
#include <thread>
#include <vector>

using ULL = unsigned long long;

// Avoid false sharing
struct AlignedULL
{
    alignas(64) ULL value = 0;
};

ULL parallel_accumulate(std::vector<ULL> &);

int main()
{

    const size_t LENGTH = 1000000000;
    std::vector<ULL> vec(LENGTH);
    for (ULL i = 0; i < LENGTH; i++)
    {
        vec[i] = i + 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    ULL result_1 = std::accumulate(vec.begin(), vec.end(), 0ULL);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Result 1: " << result_1 << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    ULL result_2 = parallel_accumulate(vec);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Result 2: " << result_2 << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    return 0;
}

void accumulate(std::vector<ULL>::iterator begin, std::vector<ULL>::iterator end, AlignedULL &result)
{
    result.value = std::accumulate(begin, end, 0ULL);
}

ULL parallel_accumulate(std::vector<ULL> &vec)
{
    const ULL num_threads = 8;
    const ULL batch_size = vec.size() / num_threads;

    std::vector<AlignedULL> results(num_threads);
    std::vector<std::thread> threads;
    threads.reserve(num_threads - 1);

    std::vector<ULL>::iterator begin = vec.begin();

    for (ULL i = 0; i < num_threads - 1; i++)
    {
        std::vector<ULL>::iterator end = begin;
        std::advance(end, batch_size);
        threads[i] = std::thread(accumulate, begin, end, std::ref(results[i]));
        begin = end;
    }
    accumulate(begin, vec.end(), results[num_threads - 1]);

    for (ULL i = 0; i < num_threads - 1; i++)
    {
        if (threads[i].joinable()) threads[i].join();
    }

    return std::accumulate(results.begin(), results.end(), 0ULL,
                           [](ULL sum, const AlignedULL &data) { return sum + data.value; });
}