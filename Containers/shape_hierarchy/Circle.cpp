#include "Circle.h"
#include <iostream>

void Circle::draw() const { std::cout << "Drawing Circle\n"; }

double Circle::area() const { return 3.14 * radius * radius; }