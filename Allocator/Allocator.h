// CustomAllocator<T> is a minimalist allocator that mirrors the essential
// responsibilities of std::allocator, but exposes the mechanics explicitly so
// memory management behavior is visible.
//
// Key purpose: separate *memory allocation* from *object construction*.
//
// Why this matters:
// - new/delete combines allocation + construction and deallocation +
// destruction.
// - Allocators split these operations, giving containers fine-grained control
// and enabling custom memory strategies.
// - Understanding this separation is fundamental for building real container
// abstractions (std::vector, std::map, etc).

#include <cstddef>
#include <limits>
#include <new>
#include <utility>

template <typename T>
class CustomAllocator {
  public:
    // Standard allocator typedefs to support allocator_traits and generic
    // container code.
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using size_type = size_t;

    CustomAllocator() = default;
    ~CustomAllocator() = default;

    // allocate(n):
    // Request raw, *uninitialized* storage for n objects of type T.
    // Does NOT invoke constructors — caller must do that explicitly.
    //
    // Reasons for checks:
    // * n == 0 → return nullptr is allowed by allocator API, avoids UB.
    // * n > max_size → prevent integer overflow (n * sizeof(T)) and signal
    // impossible allocation.
    //
    // Uses global ::operator new (not new T[n]) → because we only want raw
    // memory, not object construction.
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();
        return static_cast<pointer>(::operator new(sizeof(T) * n));
    }

    // deallocate(p, n):
    // Releases raw storage, but does NOT call destructors — caller must do that
    // beforehand. n is unused here, but some allocators need it (debug
    // allocators, sized deletes).
    void deallocate(pointer p, size_type n) { ::operator delete(p); }

    // max_size():
    // Protects against overflow in allocate(n), i.e. n * sizeof(T) must fit in
    // size_t. Ensures allocator conforms to the formal C++ allocator
    // requirements.
    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    // construct(p, args...):
    // constructs an object of type U at raw memory address p using placement
    // new.
    //
    // Why static? No allocator state is used here.
    // Perfect-forwards args so value categories (lvalue/rvalue) are preserved.
    //
    // Critical insight: object lifetime begins only after placement-new; memory
    // alone is not an object.
    template <typename U, typename... Args>
    static void construct(U *p, Args &&...args) {
        new (p) U(std::forward<Args>(args)...);
    }

    // destroy(p):
    // Explicitly calls the destructor without freeing memory.
    // Mirrors destroy in std::allocator.
    //
    // Note: Must be called before deallocate to avoid leaking object resources.
    template <typename U>
    void destroy(U *p) {
        p->~U();
    }

    // Converting copy constructor:
    // Allows allocator<T> to construct from allocator<U>, enabling allocator
    // "rebind". Vector<T> may need to create internal allocator<U> for U ≠ T.
    // Stateless allocator → no actual data copied.
    template <typename U>
    CustomAllocator(const CustomAllocator<U> &) noexcept {}
};
