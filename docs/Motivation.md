# Motivation
`std::basic_string` is *the* C++ string class (template) and is intended for general purpose
usage. Some use cases, however, cannot be implemented with `std::basic_string`, because it has
some drawbacks:
* `std::basic_string` allocates memory on the heap, which makes `std::basic_string` an
  "improper" replacement for "plain old C" character arrays.
* `std::basic_string` allocates and reallocates data as required, transparently growing (or
  potentially shrinking) the reserved memory area.


## Why are heap allocations bad?

First of all: they're not bad. They are actually great, because `std::basic_string` can grow
almost unlimited, so you can store strings at all lengths and don't have to worry that they are
longer than any arbitrary limit.

But there is a small caveat: Allocating memory on the heap is a performance penalty compared to
"allocating" memory on the stack. For most use cases, this penalty is worth it. Current
implementations of `std::basic_string` try to avoid heap allocations for short strings
(Short/Small String Optimizations, SSO), but this is usually limited to lengths < 30 characters.

Using the stack instead of the heap however limits the string to a fixed maximum length, because
data on the stack cannot be resized on demand. But if you either know a maximum length or can
"live" with one in order to get that last bit of performance, a stack-based string class is
an interesting alternative. This is were `spsl::ArrayString<N>` starts to look nicer...

A real-life example that motivated me to write the precursor of `spsl::ArrayString<N>` are
SQL database access layers: If you have columns in your SQL database with a maximum length,
you can use the same limit in your code. For example, if your "lastname" colum in the "persons"
table is defined as `VARCHAR(50)`, you know a maximum string length and can take advantage
of this fact to skip heap allocations. That's why there is `spsl::ArrayString<N>`.


## What's wrong with reallocations?

This also isn't a bad thing per-se. It's actually a great feature for most usage patterns:
Let's say you need N bytes of data from the heap. You allocate it from the OS and fill it with
your data. Next thing you know is that you need more space because you need to append more
data. You simply call `std::basic_string::append` and you're done.

In this case, two things can happen (assuming you're not ouf of memory):
If you still have enough unused capacity left, the new data is appended and "almost nothing"
happens. This often is the case because `std::basic_string` implementations usually
allocate more memory than initially requested to avoid reallocations.
But if the remaining space is to small, a new memory area is allocated, all your data
is copied. The string objects releases the old memory are and will use the new one from now on.
The old memory can now be used by other processes. Again - very useful.

But there is one caveat: The memory areas that are released aren't cleaned up. For at least 99%
of all use cases, that's not a problem. But if you stored sensitive data in this memory region,
e.g. the password for your cloud provider, this data is still there. And now, other processes
can see it, because if they allocate memory on the heap, they might get the region with your old
data - because you don't need it anymore for your string.

The only solution is to manually "clean up" or "wipe" the memory you used before deallocating it.
That's what `spsl::PasswordString[W]` does (and more...).


# Why not use allocators?

After reading the arguments above, you might argue: "But this is what `std::allocator` was
designed for, right? Just write an allocator that uses an array on the stack / wipes memory
before deallocating it and use this with `std::basic_string`. You don't need to implement
another string class template!"

My answer to this is sad and short: I tried, but I failed. But what kept me going was: It's
not my fault :-)

The reason why both use cases cannot be implemented with recent `std::basic_string`
implementations are the already mentioned Short String Optimizations:
These optimizations work by cleverly using some of the stack space, that is already required
to store a `std::basic_string` object, and put short strings there.
The actual implementation doesn't really matter in this case - simply imagine that someone
found a "clever hack" to mix meta-information and data. The great thing about this: no heap
allocation, more speed.

But there are 2 problems, why this defeats any `std::allocator` implementations:
* We cannot simply write an allocator that wipes memory before deallocating it, because there
  might not be any allocation at all. With SSO, a short password may directly be stored on the
  stack - outside of the reach of an allocator. In order to wipe this memory as well,
  `std::basic_string` needs to be modified, which is not an option.
* The `max_size()` limitation of the allocator isn't directly respected in `std::basic_string`
  implementations. This means that the maximum capacity that an allocator implementation can
  provide is either ignored or further reduced, making an allocator implementation based on a 
  fixed-size array impossible.

To provide more background for the second point, we need to take a look at the different STL
implementations (at leat the relevant parts):

*GCC 4.9*

    _Rep::_S_max_size = (((npos - sizeof(_Rep_base))/sizeof(_CharT))
    [...]
    ///  Returns the size() of the largest possible %string.
    size_type
    max_size() const _GLIBCXX_NOEXCEPT
    { return _Rep::_S_max_size; }
    
The allocator's `max_size()` is ignored, thus calling `max_size()` on a string wouldn't return
the actual size limit. An API that provides incorrect values is *not* an option for production code.

*GCC 5.4*

    ///  Returns the size() of the largest possible %string.
    size_type
    max_size() const _GLIBCXX_NOEXCEPT
    { return (_Alloc_traits::max_size(_M_get_allocator()) - 1) / 2; } 

Here, the allocator's limit is respected, but it's cut in half (TODO: why?).
Thus, if the allocator would have space for N characters, only (N-1)/2 would be used.
To solve this, the allocator must either over-allocate memory - waisting half of it's space -
or "lie" about it's `max_size()`. Both aren't an option for production code.

*libc++ (LLVM)*

    template <class _CharT, class _Traits, class _Allocator>
    inline _LIBCPP_INLINE_VISIBILITY
    typename basic_string<_CharT, _Traits, _Allocator>::size_type
    basic_string<_CharT, _Traits, _Allocator>::max_size() const _NOEXCEPT
    {
        size_type __m = __alloc_traits::max_size(__alloc());
    #if _LIBCPP_BIG_ENDIAN
        return (__m <= ~__long_mask ? __m : __m/2) - __alignment;
    #else
        return __m - __alignment;
    #endif
    }

In this implementation, alignment is taken into account. This also reduces the allocator's maximum
capacity and would also require either over-allocation or an "dishonest" API.
Also, for big endian machines, the limit may also be cut in half to reserve the highest bit
in the capacity for SSO.

Unfortunatly, this forces a re-implementation of the `std::basic_string` template in order
to cover the use cases described above.
