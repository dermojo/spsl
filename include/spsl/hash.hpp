/**
 * @file    Special Purpose Strings Library: hash
 * @author  Daniel Evers
 * @brief   Hashing implementation to enable std::hash specialization
 * @license MIT
 *
 * The hashing function is MurmurHash3, taken thankfully from https://github.com/aappleby/smhasher.
 * It provides a 32 bit hash and a 128 bit hash, but no 64 bit version. This is handled by using
 * the 128 bit hash and cutting it to 64 bit.
 */

#ifndef SPSL_HASH_HPP_
#define SPSL_HASH_HPP_

#include <cstdint>
#include <cstring>

namespace spsl
{
namespace hash
{
namespace murmurhash3
{
// Copied from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp with
// small adaptations for C++. Thanks!

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE __forceinline

#include <cstdlib>

#define ROTL32(x, y) _rotl(x, y)
#define ROTL64(x, y) _rotl64(x, y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else // defined(_MSC_VER)

#define FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}

#define ROTL32(x, y) rotl32(x, y)
#define ROTL64(x, y) rotl64(x, y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint32_t getblock32(const uint32_t* p, uint32_t i)
{
    return p[i];
}

FORCE_INLINE uint64_t getblock64(const uint64_t* p, std::size_t i)
{
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

//----------

FORCE_INLINE uint64_t fmix64(uint64_t k)
{
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

//-----------------------------------------------------------------------------

inline void MurmurHash3_x86_32(const void* key, const uint32_t len, uint32_t seed, void* out)
{
    auto data = reinterpret_cast<const uint8_t*>(key);
    const uint32_t nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    //----------
    // body

    auto blocks = reinterpret_cast<const uint32_t*>(data);

    for (uint32_t i = 0; i < nblocks; ++i)
    {
        uint32_t k1 = getblock32(blocks, i);

        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    const uint8_t* tail = (data + nblocks * 4);

    uint32_t k1 = 0;

    switch (len & 3)
    {
    case 3:
        k1 ^= static_cast<uint32_t>(tail[2] << 16);
    /* no break */
    case 2:
        k1 ^= static_cast<uint32_t>(tail[1] << 8);
    /* no break */
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);

    *static_cast<uint32_t*>(out) = h1;
}

//-----------------------------------------------------------------------------

inline void MurmurHash3_x64_128(const void* key, const std::size_t len, const uint32_t seed,
                                void* out)
{
    auto data = reinterpret_cast<const uint8_t*>(key);
    const std::size_t nblocks = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
    const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

    //----------
    // body

    auto blocks = reinterpret_cast<const uint64_t*>(data);

    for (std::size_t i = 0; i < nblocks; i++)
    {
        uint64_t k1 = getblock64(blocks, i * 2 + 0);
        uint64_t k2 = getblock64(blocks, i * 2 + 1);

        k1 *= c1;
        k1 = ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;

        h1 = ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;

        h2 = ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    //----------
    // tail

    const uint8_t* tail = (data + nblocks * 16);

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15)
    {
    case 15:
        k2 ^= (static_cast<uint64_t>(tail[14])) << 48;
    /* no break */
    case 14:
        k2 ^= (static_cast<uint64_t>(tail[13])) << 40;
    /* no break */
    case 13:
        k2 ^= (static_cast<uint64_t>(tail[12])) << 32;
    /* no break */
    case 12:
        k2 ^= (static_cast<uint64_t>(tail[11])) << 24;
    /* no break */
    case 11:
        k2 ^= (static_cast<uint64_t>(tail[10])) << 16;
    /* no break */
    case 10:
        k2 ^= (static_cast<uint64_t>(tail[9])) << 8;
    /* no break */
    case 9:
        k2 ^= (static_cast<uint64_t>(tail[8])) << 0;
        k2 *= c2;
        k2 = ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;
    /* no break */

    case 8:
        k1 ^= (static_cast<uint64_t>(tail[7])) << 56;
    /* no break */
    case 7:
        k1 ^= (static_cast<uint64_t>(tail[6])) << 48;
    /* no break */
    case 6:
        k1 ^= (static_cast<uint64_t>(tail[5])) << 40;
    /* no break */
    case 5:
        k1 ^= (static_cast<uint64_t>(tail[4])) << 32;
    /* no break */
    case 4:
        k1 ^= (static_cast<uint64_t>(tail[3])) << 24;
    /* no break */
    case 3:
        k1 ^= (static_cast<uint64_t>(tail[2])) << 16;
    /* no break */
    case 2:
        k1 ^= (static_cast<uint64_t>(tail[1])) << 8;
    /* no break */
    case 1:
        k1 ^= (static_cast<uint64_t>(tail[0])) << 0;
        k1 *= c1;
        k1 = ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    static_cast<uint64_t*>(out)[0] = h1;
    static_cast<uint64_t*>(out)[1] = h2;
}

//-----------------------------------------------------------------------------
}

using std::size_t;

template <size_t HashLength>
size_t hash_impl(const void*, size_t);

/**
 * 32 bit hash function: This function shall be called on 32 bit x86.
 * @param[in] buffer        the data to hash
 * @param[in] len           the number of bytes in the buffer
 * @return 32 bit hash value
 */
template <>
inline size_t hash_impl<32>(const void* buffer, size_t len)
{
    uint32_t result = 0;
    murmurhash3::MurmurHash3_x86_32(buffer, static_cast<uint32_t>(len), 0 /* seed */, &result);
    return result;
}

/**
 * 64 bit hash function: This function shall be called on 64 bit x86_64.
 * @param[in] buffer        the data to hash
 * @param[in] len           the number of bytes in the buffer
 * @return 64 bit hash value
 */
template <>
inline size_t hash_impl<64>(const void* buffer, size_t len)
{
    uint8_t result128[16];
    murmurhash3::MurmurHash3_x64_128(buffer, len, 0 /* seed */, result128);

    // this is the only "defined" way to convert properly...
    size_t result = 0;
    memcpy(&result, result128, sizeof(result));
    return result;
}

/**
 * System-independent hash function: Call this function to hash something. Based on the current
 * platform, the matching hash function is called.
 * @param[in] buffer        the data to hash
 * @param[in] len           the number of bytes in the buffer
 * @return the calculated hash
 */
inline std::size_t hash_impl(const void* buffer, size_t len)
{
    return hash_impl<sizeof(size_t) * 8>(buffer, len);
}
}
}


#endif /* SPSL_HASH_HPP_ */
