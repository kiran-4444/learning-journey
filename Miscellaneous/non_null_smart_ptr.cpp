#include <cassert>
#include <iostream>
#include <stdexcept>
template <typename T>
class non_null_ptr
{
    T *ptr;

  public:
    explicit non_null_ptr(T *pointer)
    {
        if (!pointer) throw std::invalid_argument{"null pointer passed"};
        ptr = pointer;
    }

    non_null_ptr(const non_null_ptr &) = default;
    non_null_ptr &operator=(const non_null_ptr &) = default;
    non_null_ptr(non_null_ptr &&) = default;
    non_null_ptr &operator=(non_null_ptr &&) = default;

    ~non_null_ptr() = default;

    T &operator*() const noexcept { return *ptr; }
    T *operator->() const noexcept { return ptr; }
    T *get() const noexcept { return ptr; }
};

struct Dummy
{
    int x;
    void increment() { ++x; }
};

void test_null_construction_throws()
{
    std::cout << "\tTesting constructor with nullptr..." << std::endl;
    bool caught = false;
    try
    {
        non_null_ptr<Dummy> ptr(nullptr);
    }
    catch (const std::invalid_argument &)
    {
        caught = true;
    }
    assert(caught && "Expected constructor to throw for nullptr");
}

void test_dereference_and_member_access()
{
    std::cout << "\tTesting dereferencing..." << std::endl;
    Dummy d{10};
    non_null_ptr<Dummy> ptr(&d);
    assert((*ptr).x == 10);
    ptr->increment();
    assert(d.x == 11);
}

void test_copy_construction_and_assignment()
{
    std::cout << "\tTesting copy constructor and assignment..." << std::endl;
    Dummy d{5};
    non_null_ptr<Dummy> p1(&d);
    non_null_ptr<Dummy> p2 = p1; // copy construction
    assert(p2.get() == p1.get());
    assert(&(*p1) == &(*p2));
}

void test_move_construction_and_assignment()
{
    std::cout << "\tTesting move constructor and assignment..." << std::endl;
    Dummy d{42};
    non_null_ptr<Dummy> p1(&d);
    non_null_ptr<Dummy> p2 = std::move(p1); // move construction
    assert(p2.get() == &d);
    non_null_ptr<Dummy> p3(&d);
    p3 = std::move(p2); // move assignment
    assert(p3.get() == &d);
}

int main()
{
    std::cout << "Running tests..." << std::endl;
    test_null_construction_throws();
    test_dereference_and_member_access();
    test_copy_construction_and_assignment();
    test_move_construction_and_assignment();

    std::cout << "All tests passed.\n";
    return 0;
}
