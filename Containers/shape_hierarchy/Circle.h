#pragma once
#include "Shape.h"

class Circle : public Shape {
  public:
    Circle(double r) : radius(r) {};

    void draw() const override;
    double area() const override;

    std::string name() const override { return "Circle"; }

  private:
    double radius;
};