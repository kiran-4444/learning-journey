#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <new>
#include <ostream>
#include <vector>

inline bool is_aligned(const volatile void *p, std::size_t n) { return reinterpret_cast<std::uintptr_t>(p) % n == 0; }

template <size_t N>
class Arena {
  private:
    alignas(std::max_align_t) std::array<std::byte, N> buffer;
    std::byte *next;

  public:
    Arena() noexcept : next(&buffer[0]) { std::cout << "Initializing arena..." << std::endl; };
    Arena(const Arena &) = delete;
    Arena &operator=(const Arena &) = delete;

    ~Arena() noexcept { std::cout << std::endl << "Freeing arena..." << std::endl; }

    const size_t used() noexcept { return static_cast<size_t>(next - &buffer[0]); }

    size_t align_up(std::uintptr_t p, size_t alignment) {
        if (p % alignment == 0) {
            return p;
        } else {
            return p + alignment - (p % alignment);
        }
    }

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

    void deallocate(std::byte *p, size_t n) noexcept {
        std::cout << std::endl;
        std::cout << "Deallocating..." << std::endl;
        next = &buffer[0];
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

    void deallocate(pointer p, size_type n) {
        // no op
    }

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
