#include "../Vector.h"
#include <chrono>
#include <iostream>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

template <typename Func>
long long time_it(Func f)
{
    auto start = Clock::now();
    f();
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
        .count();
}

int main()
{
    const int N = 1e7;

    // 1) Your Vector — no reserve
    long long t1 = time_it([&]() {
        Vector<int> v;
        for (int i = 0; i < N; i++) {
            v.push_back(i);
        }
    });

    // 2) Your Vector — with reserve
    long long t2 = time_it([&]() {
        Vector<int> v;
        v.reserve(N);
        for (int i = 0; i < N; i++) {
            v.push_back(i);
        }
    });

    // 3) std::vector — no reserve
    long long t3 = time_it([&]() {
        std::vector<int> v;
        for (int i = 0; i < N; i++) {
            v.push_back(i);
        }
    });

    // 4) std::vector — with reserve
    long long t4 = time_it([&]() {
        std::vector<int> v;
        v.reserve(N);
        for (int i = 0; i < N; i++) {
            v.push_back(i);
        }
    });

    std::cout << "N = " << N << "\n";
    std::cout << "Your Vector  (no reserve): " << t1 << " ms\n";
    std::cout << "Your Vector  (reserve N): " << t2 << " ms\n";
    std::cout << "std::vector  (no reserve): " << t3 << " ms\n";
    std::cout << "std::vector  (reserve N): " << t4 << " ms\n";
}
