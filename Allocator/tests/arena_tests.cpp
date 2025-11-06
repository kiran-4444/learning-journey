#include "../Arena.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

void test_alignment() {
    std::cout << "  Testing alignment..." << std::endl;
    Arena<1024> arena;
    std::byte *ptr = arena.allocate(1, 3); // Offset = 3
    assert(ptr != nullptr);

    std::byte *aligned_ptr = arena.allocate(4, 4); // Offset = 8
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(aligned_ptr);
    assert(addr % 4 == 0);

    assert(arena.used() == 8);
}

void test_stl_compatibility() {
    std::cout << "  Testing STL Compatibility..." << std::endl;
    Arena<1024> arena;
    std::vector<int, CustomAllocator<int, 1024>> vec_1(arena);
    std::vector<int, CustomAllocator<int, 1024>> vec_2(arena);

    vec_1.push_back(1);
    vec_2.push_back(2);

    assert(vec_1[0] == 1);
    assert(vec_2[0] == 2);
    assert(arena.used() == 8);

    vec_1.clear();
    // This will not trigger deallocate at arena level, this will only deallocate at vector level.
    vec_1.shrink_to_fit();

    assert(vec_2[0] == 2);
    assert(arena.used() == 8);
}

int main() {
    test_alignment();
    test_stl_compatibility();
}