#include "../Allocator.h"
#include <cassert>
#include <iostream>
#include <vector>

class TestObject {

  public:
    int id;
    static int construction_count;
    static int destruction_count;

    TestObject() : id(-1) { construction_count++; }
    TestObject(int i) : id(i) { construction_count++; }
    ~TestObject() { destruction_count++; }
};

int TestObject::construction_count = 0;
int TestObject::destruction_count = 0;

template <typename T, typename U>
bool operator==(const CustomAllocator<T> &, const CustomAllocator<U> &) {
    return true;
}

template <typename T, typename U>
bool operator!=(const CustomAllocator<T> &, const CustomAllocator<U> &) {
    return false;
}

int main() {
    std::cout << "Running tests for CustomAllocator..." << std::endl;
    {
        std::cout << "  Test 1: Simple allocate/deallocate..." << std::endl;
        CustomAllocator<int> alloc_int;

        int *ptr_zero = alloc_int.allocate(0);
        assert(ptr_zero == nullptr && "n = 0 allocation failed");

        int *p = alloc_int.allocate(10);
        assert(p != nullptr && "n = 10 allocation failed");

        alloc_int.deallocate(p, 10);
    }
    {
        std::cout << "  Test 2: max_size()..." << std::endl;
        CustomAllocator<int> alloc_int;
        assert(alloc_int.max_size() ==
                   std::numeric_limits<std::size_t>::max() / sizeof(int) &&
               "max_size() for int failed");

        CustomAllocator<char> alloc_char;
        assert(alloc_char.max_size() ==
                   std::numeric_limits<std::size_t>::max() / sizeof(char) &&
               "max_size() for char failed");

        CustomAllocator<TestObject> alloc_test_object;
        assert(alloc_test_object.max_size() ==
                   std::numeric_limits<std::size_t>::max() /
                       sizeof(TestObject) &&
               "max_size() for TestObject failed");
    }
    {
        std::cout << "  Test 3: construct and destroy lifecycle..."
                  << std::endl;
        CustomAllocator<TestObject> alloc_test_object;

        TestObject::construction_count = 0;
        TestObject::destruction_count = 0;

        TestObject *test_object_ptr = alloc_test_object.allocate(1);
        assert(test_object_ptr != nullptr);

        // No construction yet, just memory allocation
        assert(TestObject::construction_count == 0);

        alloc_test_object.construct(test_object_ptr, 123);
        assert(TestObject::construction_count == 1);
        // Test for perfect forwarding
        assert(test_object_ptr->id == 123);

        alloc_test_object.destroy(test_object_ptr);
        assert(TestObject::destruction_count == 1);

        // Deallocate the memory
        alloc_test_object.deallocate(test_object_ptr, 1);
    }
    {
        std::cout << "  Test 4: Rebinding constructor..." << std::endl;
        CustomAllocator<int> alloc_int;

        CustomAllocator<double> alloc_double(alloc_int);

        double *ptr_double = alloc_double.allocate(1);
        assert(ptr_double != nullptr);
        alloc_double.deallocate(ptr_double, 1);
    }
    {
        std::cout << "  Test 5: std::vector integration..." << std::endl;

        std::vector<std::string, CustomAllocator<std::string>> vec;

        vec.push_back("Hello");
        vec.push_back("World");
        vec.push_back("!");

        assert(vec.size() == 3);
        assert(vec[0] == "Hello");
        assert(vec[1] == "World");
        assert(vec[2] == "!");

        vec.pop_back();
        assert(vec.size() == 2);
    }
    std::cout << "\nAll tests passed successfully!" << std::endl;
    return 0;
}