#include <cassert>
#include <gtest/gtest.h>
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

TEST(NonNullPtrTest, ThrowsOnNull) { EXPECT_THROW(non_null_ptr<Dummy>(nullptr), std::invalid_argument); }
TEST(NonNullPtrTest, DeferenceAndAccess)
{
    Dummy d{10};
    non_null_ptr<Dummy> ptr(&d);
    EXPECT_EQ(ptr->x, 10);
    ptr->increment();
    EXPECT_EQ(d.x, 11);
}
TEST(NonNullPtrTest, CopyAndMove)
{
    Dummy d{42};
    non_null_ptr<Dummy> p1(&d);
    non_null_ptr<Dummy> p2 = p1;
    EXPECT_EQ(p1.get(), p2.get());

    non_null_ptr<Dummy> p3 = std::move(p2);
    EXPECT_EQ(p3.get(), &d);
}