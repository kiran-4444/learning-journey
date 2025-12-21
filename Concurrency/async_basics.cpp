#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int heavy_computation(int x)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return x * x;
}

int main()
{
    std::cout << "Starting async_task..." << std::endl;
    std::future<int> result = std::async(heavy_computation, 10);

    std::cout << "Main thread is free!" << std::endl;

    for (int i = 0; i < 5; i++)
    {

        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << std::endl;

    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}