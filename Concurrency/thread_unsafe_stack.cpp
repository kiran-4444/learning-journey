#include <iostream>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

std::mutex mu;

int main()
{
    std::stack<int> stack;

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
    {
        threads.push_back(std::thread(
            [&stack, i]()
            {
                // Remove this line and run: valgring --tool=helgrind ./a.out, you'll
                // see possible data races
                std::lock_guard<std::mutex> lk(mu);
                std::cout << i << std::endl;
                stack.emplace(i);
            }));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
    return 0;
}