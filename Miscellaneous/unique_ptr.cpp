#include <iostream>
#include <memory> // for std::unique_ptr

class Resource {
  public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
};

int main() {
    // this will only acquire the resource but never destroy
    // Resource *bad_res = new Resource();

    // allocate a Resource object and have it owned by std::unique_ptr
    // this will automatically deallocate the resource once res goes out of scope
    std::unique_ptr<Resource> res{new Resource()};

    return 0;
} // res goes out of scope here, and the allocated Resource is destroyed