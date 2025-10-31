#include "../Vector.h"
#include <cassert>
#include <stdexcept>

using namespace std;

int main() {
    Vector v_1(3);
    v_1[0] = 1;
    v_1[1] = 123;
    v_1[2] = 234;

    assert(v_1.size() == 3 && "size() failed");

    assert(v_1[0] == 1 && "invalid offset value");
    assert(v_1[1] == 123 && "invalid offset value");
    assert(v_1[2] == 234 && "invalid offset value");

    try
    {
        Vector v_2(-23);
    }
    catch(const std::length_error&)
    {

    }
    catch(...)
    {
        assert(false && "Wrong exception type");
    }


    try
    {
        v_1[3];
    }
    catch(const std::out_of_range&)
    {

    }
    catch(...)
    {
        assert(false && "Wrong exception type");
    }

}