#pragma once

#include "../../Allocator/Allocator.h"
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

template <typename T, typename Allocator = CustomAllocator<T>>
class Vector {
  private:
    Allocator alloc;
    T *elem;
    int sz;
    int cap;

  public:
    explicit Vector(int sz);
    explicit Vector();
    Vector(std::initializer_list<T> lst);
    ~Vector() {
        for (int i = 0; i < sz; i++)
            alloc.destroy(&elem[i]);
        if (elem) alloc.deallocate(elem, cap);
    }

    Vector(const Vector &other);
    Vector &operator=(const Vector &other);

    Vector(Vector &&other) noexcept;
    Vector &operator=(Vector &&other) noexcept;

    T &operator[](int i);
    const T &operator[](int i) const;

    T *begin() { return elem; }
    T *end() { return elem + sz; }

    const T *begin() const { return elem; }
    const T *end() const { return elem + sz; }

    int size() const noexcept { return sz; }
    int capacity() const noexcept { return cap; }
    void print();

    void push_back(const T &);
    void reserve(int new_cap);
};

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector() {
    // elem = new T[0]();
    elem = alloc.allocate(0);
    this->sz = 0;
    this->cap = 0;
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(int sz) {
    if (sz < 0) {
        throw std::length_error{"error"};
    }

    // elem = new T[sz]();
    elem = alloc.allocate(sz);
    for (int i = 0; i < sz; i++)
        alloc.construct(&elem[i], T{});
    this->sz = sz;
    this->cap = sz;
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::initializer_list<T> lst) :
    elem{nullptr},
    sz{0},
    cap{0} {
    sz = cap = lst.size();
    elem = alloc.allocate(cap);
    int i = 0;
    for (const auto &v : lst) {
        alloc.construct(&elem[i++], v);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector &other) :
    elem{nullptr},
    sz{0},
    cap{0} {
    sz = cap = other.sz;
    elem = alloc.allocate(cap);
    for (int i = 0; i < sz; i++)
        alloc.construct(&elem[i], other.elem[i]);
}

template <typename T, typename Allocator>
Vector<T, Allocator> &Vector<T, Allocator>::operator=(const Vector &other) {
    if (this != &other) {
        for (int i = 0; i < sz; i++)
            alloc.destroy(&elem[i]);
        if (elem) alloc.deallocate(elem, cap);

        sz = other.sz;
        cap = other.cap;
        elem = alloc.allocate(cap);
        for (int i = 0; i < sz; i++)
            alloc.construct(&elem[i], other.elem[i]);
    }

    return *this;
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector &&other) noexcept :
    elem{other.elem},
    sz{other.sz},
    cap{other.cap} {
    other.elem = nullptr;
    other.sz = 0;
    other.cap = 0;
}

template <typename T, typename Allocator>
Vector<T, Allocator> &Vector<T, Allocator>::operator=(Vector &&other) noexcept {
    if (this != &other) {
        for (int i = 0; i < sz; i++)
            alloc.destroy(&elem[i]);
        if (elem) alloc.deallocate(elem, cap);

        elem = other.elem;
        sz = other.sz;
        cap = other.cap;

        other.elem = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    return *this;
}

template <typename T, typename Allocator>
T &Vector<T, Allocator>::operator[](int i) {
    if (i < 0 || i >= sz) {
        throw std::out_of_range{"Vector::operator[]"};
    }

    return elem[i];
}

template <typename T, typename Allocator>
const T &Vector<T, Allocator>::operator[](int i) const {
    if (i < 0 || i >= sz) {
        throw std::out_of_range{"Vector::operator[]"};
    }

    return elem[i];
}

template <typename T, typename Allocator>
std::ostream &operator<<(std::ostream &os, const Vector<T, Allocator> &v) {
    os << "{ ";
    for (int i = 0; i < v.size(); i++) {
        os << v[i];

        if (i + 1 < v.size()) {
            os << ", ";
        }
    }

    os << " }";
    return os;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::print() {
    for (int i = 0; i < sz; i++) {
        std::cout << elem[i] << '\n';
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::push_back(const T &value) {
    if (sz == cap) {
        reserve(cap == 0 ? 1 : cap * 2);
    }

    elem[sz] = value;
    sz += 1;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::reserve(int new_cap) {
    if (new_cap <= cap) {
        return;
    }

    T *p = alloc.allocate(new_cap);

    for (int i = 0; i < sz; i++) {
        alloc.construct(&p[i], elem[i]);
        alloc.destroy(&elem[i]);
    }
    if (elem) alloc.deallocate(elem, cap);
    elem = p;
    cap = new_cap;
}

template <typename Container, typename Value>
Value sum(const Container &c, Value v) {
    for (auto x : c) {
        v += x;
    }

    return v;
}
