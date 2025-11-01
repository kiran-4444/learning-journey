#pragma once
#include "Shape.h"

class Rectangle : public Shape {
  public:
    Rectangle(double w, double h) : width(w), height(h) {};

    void draw() const override;
    double area() const override;
    std::string name() const override { return "Rectangle"; }

  private:
    double width;
    double height;
};