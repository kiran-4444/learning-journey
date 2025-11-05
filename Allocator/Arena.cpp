#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <new>
#include <ostream>
#include <vector>

template <size_t N>
class Arena {
  private:
    std::array<std::byte, N> buffer;
    std::byte *next;

  public:
    Arena() noexcept : next(&buffer[0]) {
        std::cout << "Initializing arena..." << std::endl;
    };
    Arena(const Arena &) = delete;
    Arena &operator=(const Arena &) = delete;

    ~Arena() noexcept { std::cout << "Freeing arena..." << std::endl; }

    const size_t used() noexcept {
        return static_cast<size_t>(next - &buffer[0]);
    }

    std::byte *allocate(size_t n) {
        std::cout << "Requested bytes: " << n << std::endl;
        std::cout << "Used space: " << used() << std::endl;
        size_t bytes_free = next - &buffer[0];
        std::cout << "Free bytes: " << bytes_free << std::endl;

        if (bytes_free > n) {
            std::byte *current = next;
            next = next + n;
            return current;
        } else {
            std::bad_alloc();
        }
    }

    void deallocate(std::byte *p, size_t n) noexcept {
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
        return reinterpret_cast<pointer>(
            arena->allocate(n * sizeof(value_type)));
    }
    template <class U>
    CustomAllocator(const CustomAllocator<U, N> &other) noexcept :
        arena{other.arena} {}

    void deallocate(pointer p, size_type n) {
        arena->deallocate(reinterpret_cast<std::byte *>(p),
                          n * sizeof(value_type));
    }

    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

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

  private:
    arena_type *arena;
};

const size_t ARENA_SIZE = 1024;

int main() {
    CustomAllocator<int, ARENA_SIZE>::arena_type arena{};
    std::vector<int, CustomAllocator<int, ARENA_SIZE>> vec{arena};

    vec.push_back(1);
}