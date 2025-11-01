#pragma once
#include <string>

class Shape {
  public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};