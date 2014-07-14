#ifndef PYMEM_MALLOC_ALLOCATOR_HPP
#define PYMEM_MALLOC_ALLOCATOR_HPP

#include <Python.h>

#include <memory>
#include <limits>

template<typename T>
class PyMemMallocAllocator 
{
public : 
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    template<typename U>
    struct rebind 
    {
        typedef PyMemMallocAllocator<U> other;
    };

public : 
    inline 
    PyMemMallocAllocator() {}
    inline 
    ~PyMemMallocAllocator() {}
    inline 
    PyMemMallocAllocator(PyMemMallocAllocator const&) {}
    template<typename U>
    inline 
    PyMemMallocAllocator(PyMemMallocAllocator<U> const&) {}
    
    inline pointer 
    allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) 
    { 
        pointer p = pointer(PyMem_Malloc(cnt * sizeof(T)));
        if(p == NULL)
            throw std::bad_alloc();
        return p;
    }
    
    inline void 
    deallocate(pointer p, size_type) 
    { 
        PyMem_Free(p);    
    }

    inline void 
    construct(pointer p, const T& t) 
    { 
        new(p) T(t); 
    }
    
    inline void 
    destroy(pointer p) 
    { 
        p->~T(); 
    }

    inline size_type 
    max_size() const 
    { 
        return std::numeric_limits<size_type>::max(); 
    }
};    

template<typename T, typename T2>
inline bool 
operator==(PyMemMallocAllocator<T> const&, PyMemMallocAllocator<T2> const&) 
{ 
    return true;
}

template<typename T, typename OtherAllocator>
inline bool 
operator==(PyMemMallocAllocator<T> const &, OtherAllocator const &) 
{ 
    return false; 
}

#endif // #ifndef PYMEM_MALLOC_ALLOCATOR_HPP

