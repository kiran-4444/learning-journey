#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class ThreadSafeQueue
{
  private:
    std::mutex mut;
    std::queue<T> data_queue;
    size_t max_size;

    std::condition_variable queue_empty;
    std::condition_variable queue_full;

  public:
    ThreadSafeQueue(size_t max_size) : max_size(max_size){};

    void push(T item)
    {
        std::unique_lock<std::mutex> lk(mut);
        queue_full.wait(lk, [this] { return data_queue.size() < max_size; });
        std::cout << "Pushing: " << item << std::endl;
        data_queue.push(item);
        queue_empty.notify_one();
        lk.unlock();
    }
    T wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        queue_empty.wait(lk, [this] { return !data_queue.empty(); });
        T chunk = data_queue.front();
        data_queue.pop();

        std::cout << "Consuming item: " << chunk << std::endl;

        if (chunk == 99)
        {
            lk.unlock();
            return -1;
        };
        queue_full.notify_one();
        lk.unlock();

        return chunk;
    }
};

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