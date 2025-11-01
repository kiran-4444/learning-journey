#include "Vector.h"

#include <initializer_list>
#include <iostream>
#include <stdexcept>

using namespace std;

Vector::Vector(int s) {
    if (s < 0) {
        throw std::length_error{"error"};
    }

    elem = new double[s];
    sz = s;
}

Vector::Vector(std::initializer_list<double> lst)
    : elem{new double[lst.size()]}, sz{static_cast<int>(lst.size())} {
    copy(lst.begin(), lst.end(), elem);
}

Vector::~Vector() { delete[] elem; }

double &Vector::operator[](int i) {
    if (i < 0 || i >= size()) {
        throw std::out_of_range{"Vector::operator[]"};
    }
    return elem[i];
}

int Vector::size() noexcept { return sz; }

void Vector::print() {
    for (int i = 0; i < sz; i++) {
        std::cout << elem[i] << "\n";
    }
}