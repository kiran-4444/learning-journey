#pragma once

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

template <typename T>
class Vector {
  private:
    T *elem;
    int sz;

  public:
    explicit Vector(int sz);
    Vector(std::initializer_list<T> lst);
    ~Vector() { delete[] elem; }

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
    void print();
};

template <typename T>
Vector<T>::Vector(int sz)
{
    if (sz < 0) {
        throw std::length_error{"error"};
    }

    elem = new T[sz];
    this->sz = sz;
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> lst) : elem{new T[lst.size()]}, sz{static_cast<int>(lst.size())}
{
    std::copy(lst.begin(), lst.end(), elem);
}

template <typename T>
Vector<T>::Vector(const Vector &other) : elem{new T[other.sz]}, sz{other.sz}
{
    for (int i = 0; i < other.sz; i++) {
        elem[i] = other.elem[i];
    }
}

template <typename T>
Vector<T> &Vector<T>::operator=(const Vector &other)
{
    if (this != &other) {
        T *p = new T[other.sz];
        for (int i = 0; i < other.sz; i++) {
            p[i] = other.elem[i];
        }

        delete[] elem;
        elem = p;
        sz = other.sz;
    }

    return *this;
}

template <typename T>
Vector<T>::Vector(Vector &&other) noexcept : elem{other.elem}, sz{other.sz}
{
    other.elem = nullptr;
    other.sz = 0;
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector &&other) noexcept
{
    if (this != &other) {
        delete[] elem;

        elem = other.elem;
        sz = other.sz;

        other.elem = nullptr;
        other.sz = 0;
    }

    return *this;
}

template <typename T>
T &Vector<T>::operator[](int i)
{
    if (i < 0 || i >= sz) {
        throw std::out_of_range{"Vector::operator[]"};
    }

    return elem[i];
}

template <typename T>
const T &Vector<T>::operator[](int i) const
{
    if (i < 0 || i >= sz) {
        throw std::out_of_range{"Vector::operator[]"};
    }

    return elem[i];
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vector<T> &v)
{
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

template <typename T>
void Vector<T>::print()
{
    for (int i = 0; i < sz; i++) {
        std::cout << elem[i] << '\n';
    }
}

template <typename Container, typename Value>
Value sum(const Container &c, Value v)
{
    for (auto x : c) {
        v += x;
    }

    return v;
}
