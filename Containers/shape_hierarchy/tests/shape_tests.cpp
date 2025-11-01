#include "../Circle.h"
#include "../Rectangle.h"
#include "../Shape.h"
#include <cassert>
#include <math.h>

int main() {
    {
        Rectangle r(3.0, 4.0);
        assert(std::abs(r.area() - 12.0) < 1e-9 && "Rectangle area failed");
        assert(r.name() == "Rectangle" && "Rectangle name mismatch");
    }
    {
        Circle c(4.0);
        assert(std::abs(c.area() - (3.14 * 4.0 * 4.0)) < 1e-9 && "Circle area failed");
        assert(c.name() == "Circle" && "Circle name mismatch");
    }
}
