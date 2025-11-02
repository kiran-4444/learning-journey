#include "../Vector.h"

#include <cassert>
#include <sstream>
#include <stdexcept>

int main()
{
    {
        try {
            // double initialization
            Vector<double> v_1 = {1.0, 2.0, 3.0};
        } catch (...) {
            assert(false && "double initialization failed");
        }
    }

    {
        try {
            Vector<char> v_2 = {'a', 'b', 'c'};

        } catch (...) {
            assert(false && "char vector initialization failed");
        }
    }

    {
        try {
            Vector<double> v_3 = {4.0, 5.0, 6.0};
            for (auto &i : v_3) {
                std::cout << i << "\n";
            }
        } catch (...) {
            assert(false && "Iteration failed");
        }
    }

    {
        // test accumulator
        try {
            Vector<int> v_4 = {1, 2, 3, 4};
            int res = sum(v_4, 0);
        } catch (...) {
            assert(false && "sum failed");
        }
    }

    {
        // test << operator
        try {
            Vector<int> v_5 = {1, 2, 3};
            std::ostringstream oss;
            oss << v_5;
            // std::cout << v_5;

            assert(oss.str() == "{ 1, 2, 3 }");
        } catch (...) {
            assert(false && "Error");
        }
    }
}