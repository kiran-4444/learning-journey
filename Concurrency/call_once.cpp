#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class SomeResource
{
  public:
    SomeResource()
    {
        std::cout << "Resource has been initialized on " << std::this_thread::get_id()
                  << std::endl;
    }
    void do_something() { std::cout << "called do_something()" << std::endl; }
};

std::shared_ptr<SomeResource> resource_ptr;
std::once_flag resource_flag;

void init_resource() { resource_ptr.reset(new SomeResource); }
void foo()
{
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
    {
        threads.emplace_back(foo);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}