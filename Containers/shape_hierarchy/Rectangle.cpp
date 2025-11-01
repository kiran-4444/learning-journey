#include "Rectangle.h"
#include <iostream>

double Rectangle::area() const { return width * height; }

void Rectangle::draw() const { std::cout << "Drawing Rectangle\n"; }