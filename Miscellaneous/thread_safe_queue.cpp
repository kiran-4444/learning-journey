#include "thread_safe_queue.h"
#include <cstdlib>
#include <thread>

void data_preperation_thread(ThreadSafeQueue<int> &queue)
{
    for (int i = 0; i < 100; i++)
    {
        queue.push(i);
    }
}

void data_processing_thread(ThreadSafeQueue<int> &queue)
{

    while (true)
    {
        int chunk = queue.wait_and_pop();

        if (chunk == -1) break;
    }
}

int main()
{
    ThreadSafeQueue<int> queue(5);
    std::thread preperation_thread =
        std::thread(data_preperation_thread, std::ref(queue));
    std::thread processing_thread =
        std::thread(data_processing_thread, std::ref(queue));

    preperation_thread.join();
    processing_thread.join();
    return 0;
}