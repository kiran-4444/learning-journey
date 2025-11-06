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

void test_wipeout_flaw() {
    std::cout << "Testing Deallocation Wipeout Flaw..." << std::endl;
    Arena<1024> arena;
    CustomAllocator<int, 1024> alloc(arena);
    std::vector<int, CustomAllocator<int, 1024>> v1(alloc);
    std::vector<int, CustomAllocator<int, 1024>> v2(alloc);

    // 1. Allocate v1. Arena 'next' moves forward.
    v1.push_back(100);
    size_t used_after_v1 = arena.used();

    // 2. Allocate v2. Arena 'next' moves forward again.
    v2.push_back(200);

    // 3. Clear v1. Vector calls allocator::deallocate().
    // YOUR BUG: This resets arena.next to &buffer[0].
    v1.clear();
    v1.shrink_to_fit(); // Force deallocation

    // 4. Arena should NOT be empty, because v2 is still alive!
    // If this assertion fails, it confirms the bug: v2's memory is now marked "free".
    assert(arena.used() > 0 && "Arena incorrectly wiped out while v2 is still active!");

    // 5. Verify corruption:
    // Allocating something new will now overwrite v2's memory.
    std::vector<int, CustomAllocator<int, 1024>> v3(alloc);
    v3.push_back(999); // Overwrites v2's data because 'next' was reset.

    assert(v2[0] == 200 && "v2 data was corrupted by v3 allocation!");

    std::cout << "Wipeout flaw test passed (if assertions didn't fire)." << std::endl;
}

int main() {
    test_wipeout_flaw();
    test_alignment();
    test_stl_compatibility();
}