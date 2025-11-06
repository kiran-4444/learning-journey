// Arena<N> is a simple arena allocator that manages a fixed size buffer.
// This acquires a chunk of memory into this buffer and stores the base ont a next pointer.
// This next pointer keeps track of the current memory usage and it keeps bumping as and when
// the memory gets acquired. This operation is quire fast compared to using `malloc` everytime
// we are in need of additional memory.

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <new>
#include <ostream>
#include <vector>

// is_aligned(p, n):
// A utility function to check whether the pointer p is aligned with address that is a mutiple of n.

inline bool is_aligned(const volatile void *p, std::size_t n) { return reinterpret_cast<std::uintptr_t>(p) % n == 0; }

template <size_t N>
class Arena {
  private:
    // The buffer is aligned as well. `max_align_t` will be guaranteed to be aligned to any scalar type.
    // This will ensure that the value of `max_align_t` can be used to offset any scalar type without misalignment
    // issues.
    alignas(std::max_align_t) std::array<std::byte, N> buffer;
    std::byte *next;

  public:
    // Arena():
    // This just creates the next pointer by pointing to the start of buffer.
    Arena() noexcept : next(&buffer[0]) { std::cout << "Initializing arena..." << std::endl; };
    // We should not have any copy constructor and copy assignment. Why?
    // If we're copying an Arena, it means what we'll have two arenas pointing to the same
    // memory chunk, which can cause issues.
    Arena(const Arena &) = delete;
    Arena &operator=(const Arena &) = delete;

    // ~Arena():
    // This is a no-op as the memory would be reclaimed after this arena goes out of scope.
    ~Arena() noexcept { std::cout << std::endl << "Freeing arena..." << std::endl; }

    const size_t used() noexcept { return static_cast<size_t>(next - &buffer[0]); }

    // align_up(p, alignment):
    // This function returns the nearest boundary to pointer address p that is aligned with alignment value.
    // For example:
    // align_up(24, 4) gives 24 as it's already aligned.
    // align_up(31, 4) gives 32, the closest value to 31 that is aligned to 4 boundary.
    size_t align_up(std::uintptr_t p, size_t alignment) {
        if (p % alignment == 0) {
            return p;
        } else {
            return p + alignment - (p % alignment);
        }
    }

    // allocate(n, alignment):
    // This function tries to allocate n bytes on the buffer that that is aligned with `alignment` boundary.
    // It first calculates the padding to the current pointer address (`next`) to align with `alignment` boundary.
    // It then pads the `n` to match the `alignment` boundary. This way, we're aligning the value to be aligned with
    // `alignment` at both the ends.
    // It checks if the memory asked(padded n) is actaully allocatable, if not we'll error out.
    // Then it bumps the current pointer `next` to the calculated padding offset. Then it allocated the required space,
    // `n` (again padded to match alignment) and finally returns the start position of the alloted space.
    std::byte *allocate(size_t n, size_t alignment) {
        std::cout << std::endl;
        std::cout << "Requested bytes: " << n << std::endl;
        std::cout << "Used space: " << used() << std::endl;

        std::uintptr_t current_addr = reinterpret_cast<std::uintptr_t>(next);
        std::uintptr_t aligned_addr = align_up(current_addr, alignment);
        size_t padding = aligned_addr - current_addr;
        size_t total_needed = n + padding;
        std::cout << "Received alignment: " << alignment << std::endl;
        std::cout << "Aligned N: " << aligned_addr << std::endl;

        size_t bytes_free = N - used();
        std::cout << "Free bytes: " << bytes_free << std::endl;

        if (bytes_free >= total_needed) {
            std::byte *aligned_ptr = next + padding;
            next = next + total_needed;
            std::cout << "is_aligned: " << is_aligned(next, alignment) << std::endl;
            return aligned_ptr;
        } else {
            throw std::bad_alloc();
        }
    }

    // deallocate():
    // This will reset the `next` pointer to start pointing to start of the buffer. This way, we'll just override the
    // previously alloted values without additional cleaning of each alloted object.
    // This should be a no-op as we'll never deallocate this arena during it's lifetime. This will create unintended
    // issues when using for multiple resources.
    // For example, assume we used this arena on two vectors and pushed elements from each. Then let's assume that first
    // vector goes out of scope. This'll trigger the deallocate on arena but we cannot clear this because we still have
    // elements from the second vector in our buffer.
    void deallocate() noexcept {
        std::cout << std::endl;
        std::cout << "Deallocating..." << std::endl;
        // next = &buffer[0];
        return;
    }
};

template <typename T, size_t N>
class CustomAllocator {
  public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using size_type = size_t;
    using arena_type = Arena<N>;

    CustomAllocator(Arena<N> &arena) noexcept : arena(&arena) {};
    ~CustomAllocator() = default;

    pointer allocate(size_type n) {
        return reinterpret_cast<pointer>(arena->allocate(n * sizeof(value_type), alignof(T)));
    }
    template <class U>
    CustomAllocator(const CustomAllocator<U, N> &other) noexcept : arena{other.arena} {}

    void deallocate(pointer p, size_type n) { arena->deallocate(); }

    size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

    template <typename U, typename... Args>
    static void construct(U *p, Args &&...args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U *p) {
        p->~U();
    }

    template <typename U>
    struct rebind {
        typedef CustomAllocator<U, N> other;
    };

    // Standard equality operators for stateful allocators
    template <class U>
    bool operator==(const CustomAllocator<U, N> &other) const noexcept {
        return arena == other.arena;
    }
    template <class U>
    bool operator!=(const CustomAllocator<U, N> &other) const noexcept {
        return arena != other.arena;
    }

    //   private:
    arena_type *arena;
};
