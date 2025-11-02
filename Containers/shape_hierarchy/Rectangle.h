#pragma once
#include "Shape.h"

class Rectangle : public Shape {
  public:
    Rectangle(double w, double h) : width(w), height(h) {};

    void draw() const override;
    double area() const override;
    std::string name() const override { return "Rectangle"; }
    void set_height(double h) { height = h; }
    void set_width(double w) { width = w; }
    double get_height() { return height; }
    double get_width() { return width; }

  private:
    double width;
    double height;
};