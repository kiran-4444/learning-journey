#include "../Circle.h"
#include "../Rectangle.h"
#include "../Shape.h"
#include <cassert>
#include <math.h>

int main() {
    {
        Rectangle r_0(3.0, 4.0);
        assert(std::abs(r_0.area() - 12.0) < 1e-9 && "Rectangle area failed");
        assert(r_0.name() == "Rectangle" && "Rectangle name mismatch");
    }
    {
        Circle c_0(4.0);
        assert(std::abs(c_0.area() - (3.14 * 4.0 * 4.0)) < 1e-9 && "Circle area failed");
        assert(c_0.name() == "Circle" && "Circle name mismatch");
    }
    {
        Rectangle r_1(1.0, 1.0);
        Rectangle r_2 = r_1;

        r_1.set_height(2.0);
        assert(r_1.get_height() != r_2.get_height() && "both heights shouldn't be equal");
    }
}
