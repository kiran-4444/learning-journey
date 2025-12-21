#include <chrono>
#include <functional>
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
    std::packaged_task<int(int)> task(heavy_computation);
    std::future<int> result = task.get_future();

    std::thread background_thread([](std::packaged_task<int(int)> t) { t(10); },
                                  std::move(task));

    background_thread.detach();

    std::cout << "Waiting for result..." << std::endl;
    std::cout << "Result: " << result.get() << std::endl;

    return 0;
}