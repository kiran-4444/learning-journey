#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>

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
    ThreadSafeQueue(size_t max_size) : max_size(max_size) {};

    void push(T item)
    {
        std::unique_lock<std::mutex> lk(mut);
        queue_full.wait(lk, [this] { return data_queue.size() < max_size; });
        // std::cout << "Pushing: " << item << std::endl;
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

        // std::cout << "Consuming item: " << chunk << std::endl;

        // if (chunk == 99)
        // {
        //     lk.unlock();
        //     return -1;
        // };
        queue_full.notify_one();
        lk.unlock();

        return chunk;
    }
};
