#include "../Vector.h"

#include <cassert>
#include <iomanip>
#include <sstream>
#include <stdexcept>

int main() {
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
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1);
            for (double &i : v_3) {
                oss << i << "\n";
            }
            assert(oss.str() == "4.0\n5.0\n6.0\n" && "Wrong formatting");
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
            assert(oss.str() == "{ 1, 2, 3 }");
        } catch (...) {
            assert(false && "Error");
        }
    }

    // test push_back

    {
        try {
            Vector<int> v_6 = {1, 2, 3};
            v_6.push_back(4); // cap = 6 (3 * 2)
            v_6.push_back(5); // cap = 6

            assert(v_6.size() == 5 && "size comparision failed");
            assert(v_6[3] == 4 && "Invalid offset access");
            assert(v_6[4] == 5 && "Invalid offset access");
            assert(v_6.capacity() == 6 && "Wrong capacity");
        } catch (...) {
            assert(false && "Error");
        }
    }

    {
        Vector<int> v_7;
        for (int i = 0; i < 10000; i++) {
            v_7.push_back(i);
        }

        assert(v_7.size() == 10000 && "Invalid size");
    }

    {
        Vector<int> v_8 = {1, 2, 3};
        v_8.push_back(4);
        Vector<int> v_9 = std::move(v_8);
        assert(v_9[3] == 4);
        assert(v_8.size() == 0);
    }

    // Iterator invalidation test
    {
        Vector<int> v_9 = {1, 2, 3};
        auto it = v_9.begin();
        v_9.push_back(4);
        assert(it != v_9.begin());
    }

    // test out of bounds
    {
        Vector<int> v_10 = {1, 2, 3};

        try {
            v_10[4];
        } catch (const std::out_of_range &e) {
            assert(true);
        } catch (...) {
            assert(false && "Out of bound test failed");
        }
    }

    std::cout << "All tests passed" << std::endl;
}