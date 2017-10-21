/**
 * @file    Special Purpose Strings Library: test_pagealloc.cpp
 * @author  Daniel Evers
 * @brief   Unit tests for the page allocator
 * @license MIT
 */

#include <array>
#include <gtest/gtest.h>

#include "spsl/pagealloc.hpp"
#include "testdata.hpp"

class PageAllocTest : public ::testing::Test
{
};

// test the constructor
TEST(PageAllocTest, ConstructorTest)
{
    spsl::SensitivePageAllocator alloc;
    ASSERT_GT(alloc.getPageSize(), 0u);
    ASSERT_TRUE(alloc.getPageSize() % spsl::SensitivePageAllocator::chunk_size == 0);
    ASSERT_TRUE(alloc.getPageSize() % spsl::SensitivePageAllocator::segment_size == 0);
    ASSERT_TRUE(alloc.getChunksPerPage() >= 1);
    // for now, I only know of Solaris with 8K pages...
    ASSERT_TRUE(alloc.getChunksPerPage() <= 2);

// note: this may fail on some exotic systems...
#if 1
    ASSERT_EQ(alloc.getPageSize(), 4096);
    ASSERT_EQ(alloc.getChunksPerPage(), 1u);
#endif
}

// test the bitmask calculation
TEST(PageAllocTest, BitmaskTest)
{
    // binary literals are actually a C++14 feature...
    auto all64 = spsl::SensitivePageAllocator::all64;
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(0), 0b0);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(1), 0b1);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(2), 0b11);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(3), 0b111);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(4), 0b1111);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(5), 0b11111);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(6), 0b111111);
    //    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(20), 0b11111111111111111111);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(0), 0x0);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(1), 0x1);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(2), 0x3);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(3), 0x7);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(4), 0xf);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(5), 0x1f);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(6), 0x3f);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(20), 0xfffff);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(32), 0xffffffff);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(63), all64 >> 1);
    ASSERT_EQ(spsl::SensitivePageAllocator::getBitmask(64), all64);
}

// allocate a segment
TEST(PageAllocTest, ManagedAllocationTest1)
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);

    // ask for a few bytes
    constexpr std::size_t size1 = 16;
    static_assert(size1 <= spsl::SensitivePageAllocator::segment_size, "oops");
    void* mem = alloc.allocate(size1);
    ASSERT_NE(mem, nullptr);

    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 1 * alloc.getChunksPerPage());
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);

    alloc.deallocate(mem, size1);

    // in the end, no allocation is left
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
}

using AllocationList = std::vector<spsl::SensitivePageAllocator::AllocationInfo>;

// note: GTest requires that this function returns void
static void performAllocations(spsl::SensitivePageAllocator& alloc, AllocationList& allocations)
{
    constexpr std::size_t size1 = 15;
    constexpr std::size_t size2 = 32;
    constexpr std::size_t size3 = 64;
    static_assert(size1 <= spsl::SensitivePageAllocator::segment_size, "oops");
    static_assert(size2 <= spsl::SensitivePageAllocator::segment_size, "oops");
    static_assert(size3 <= spsl::SensitivePageAllocator::segment_size, "oops");
    const std::array<std::size_t, 3> sizes{ size1, size2, size3 };

    // keep track of allocations to that we can free them up
    allocations.clear();

    // allocate and use one chunk
    for (size_t i = 0; i < alloc.segmentsPerChunk; ++i)
    {
        // ask for a few bytes
        std::size_t size = sizes[i % sizes.size()];
        void* mem = alloc.allocate(size);
        ASSERT_NE(mem, nullptr);

        allocations.emplace_back(mem, size);

        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 1 * alloc.getChunksPerPage());
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }

    // and now the second one
    for (size_t i = 0; i < alloc.segmentsPerChunk; ++i)
    {
        // ask for a few bytes
        std::size_t size = sizes[i % sizes.size()];
        void* mem = alloc.allocate(size);
        ASSERT_NE(mem, nullptr);

        allocations.emplace_back(mem, size);

        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 2 * alloc.getChunksPerPage());
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }
}

// allocate and deallocate multiple segments
TEST(PageAllocTest, ManagedAllocationTest2)
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);

    AllocationList allocations;
    performAllocations(alloc, allocations);

    for (auto& entry : allocations)
        alloc.deallocate(entry.addr, entry.size);

    // in the end, no allocation is left
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
}

// allocate multiple segments and release them in reverse order
TEST(PageAllocTest, ManagedAllocationTest3)
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);

    AllocationList allocations;
    performAllocations(alloc, allocations);

    std::reverse(allocations.begin(), allocations.end());
    for (auto& entry : allocations)
        alloc.deallocate(entry.addr, entry.size);

    // in the end, no allocation is left
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
}

// allocate areas large than a page
TEST(PageAllocTest, UnmanagedAllocationTest)
{
    spsl::SensitivePageAllocator alloc;
    auto pageSize = alloc.getPageSize();

    // nothing allocated yet
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);

    // allocating a page is managed
    {
        void* mem = alloc.allocate(pageSize);
        ASSERT_NE(mem, nullptr);
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 1);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
        alloc.deallocate(mem, pageSize);
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }
    // allocating more is "unmanaged"
    {
        void* mem = alloc.allocate(pageSize + 1);
        ASSERT_NE(mem, nullptr);
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 1);
        alloc.deallocate(mem, pageSize + 1);
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }

    // in the end, no allocation is left
    ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 0);
    ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
}


// test the leak check in the destructor
TEST(PageAllocTest, LeakCheckTest1)
{
    AllocationList myLeaks;
    const spsl::SensitivePageAllocator* myInstance = nullptr;
    const spsl::SensitivePageAllocator* cbInstance = nullptr;
    AllocationList cbLeaks;

    {
        spsl::SensitivePageAllocator alloc;
        myInstance = &alloc;
        // get informed about leaks from the destructor
        alloc.setLeakCallback([&](const spsl::SensitivePageAllocator* p,
                                  const spsl::SensitivePageAllocator::AllocationInfo& leak, bool) {
            cbInstance = p;
            cbLeaks.push_back(leak);
        });

        AllocationList allocations;
        performAllocations(alloc, allocations);

        myLeaks.push_back(allocations.back());
        allocations.pop_back();
        for (auto& entry : allocations)
            alloc.deallocate(entry.addr, entry.size);

        // there is one page left
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 1);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }

    // now check the leaks
    ASSERT_EQ(myLeaks.size(), cbLeaks.size());
    ASSERT_EQ(cbLeaks.size(), 1);
    ASSERT_EQ(myInstance, cbInstance);
    ASSERT_EQ(cbLeaks[0].addr, myLeaks[0].addr);
    // these aren't identical because the allocator reserves more space than required
    // ASSERT_EQ(cbLeaks[0].size, myLeaks[0].size);
    ASSERT_EQ(cbLeaks[0].size, 64);
}


// test the leak check in the destructor even more
TEST(PageAllocTest, LeakCheckTest2)
{
    AllocationList myLeaks;
    const spsl::SensitivePageAllocator* myInstance = nullptr;
    const spsl::SensitivePageAllocator* cbInstance = nullptr;
    AllocationList cbLeaks;

    {
        spsl::SensitivePageAllocator alloc;
        myInstance = &alloc;
        // get informed about leaks from the destructor
        alloc.setLeakCallback([&](const spsl::SensitivePageAllocator* p,
                                  const spsl::SensitivePageAllocator::AllocationInfo& leak, bool) {
            cbInstance = p;
            cbLeaks.push_back(leak);
        });

        AllocationList allocations;
        performAllocations(alloc, allocations);

        // 128 allocations: insert some "scattered" leaks and one contiguous
        ASSERT_EQ(allocations.size(), 128);
        std::array<std::size_t, 8> indexes{ 127, 123, 122, 121, 65, 17, 15, 0 };
        for (auto index : indexes)
        {
            // note: we iterate backwards to have stable indexes, but collect "in order"
            myLeaks.insert(myLeaks.begin(), allocations[index]);
            allocations.erase(allocations.begin() +
                              static_cast<AllocationList::difference_type>(index));
        }

        for (auto& entry : allocations)
            alloc.deallocate(entry.addr, entry.size);

        // there is one page left
        ASSERT_EQ(alloc.getNumberOfManagedAllocatedPages(), 2);
        ASSERT_EQ(alloc.getNumberOfUnmanagedAreas(), 0);
    }

    // now check the leaks
    ASSERT_GT(myLeaks.size(), cbLeaks.size());
    ASSERT_EQ(cbLeaks.size(), 6);
    ASSERT_EQ(myInstance, cbInstance);
    // 0 - 15 - 17 - 65 - 121-123 - 127
    for (std::size_t i = 0; i < 4; ++i)
    {
        ASSERT_EQ(cbLeaks[i].addr, myLeaks[i].addr);
        ASSERT_EQ(cbLeaks[i].size, 64);
    }
    // 121-123 get "merged"
    ASSERT_EQ(cbLeaks[4].addr, myLeaks[4].addr);
    ASSERT_EQ(cbLeaks[4].size, 3 * 64);
    ASSERT_EQ(cbLeaks[5].addr, myLeaks[7].addr);
    ASSERT_EQ(cbLeaks[5].size, 64);
}

// TODO: test other page sizes
