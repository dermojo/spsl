/**
 * @author  Daniel Evers
 * @brief   Unit tests for the page allocator
 * @license MIT
 */

#include <array>

#include "catch.hpp"

#include "spsl/pagealloc.hpp"
#include "testdata.hpp"


// test the constructor
TEST_CASE("SensitivePageAllocator can be constructed", "[allocator]")
{
    spsl::SensitivePageAllocator alloc;
    REQUIRE(alloc.getPageSize() % spsl::SensitivePageAllocator::chunk_size == 0);
    REQUIRE(alloc.getPageSize() % spsl::SensitivePageAllocator::segment_size == 0);
    REQUIRE(alloc.getChunksPerPage() >= 1);
    // for now, I only know of Solaris with 8K pages...
    REQUIRE(alloc.getChunksPerPage() <= 2);

    // note: this may fail on some exotic systems...
    REQUIRE(alloc.getPageSize() == 4096u);
    REQUIRE(alloc.getChunksPerPage() == 1u);
}

// test the bitmask calculation
TEST_CASE("BitMask tests", "[allocator]")
{
    // binary literals are actually a C++14 feature...
    auto all64 = spsl::SensitivePageAllocator::all64;
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(0) == 0b0);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(1) == 0b1);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(2) == 0b11);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(3) == 0b111);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(4) == 0b1111);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(5) == 0b11111);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(6) == 0b111111);
    //    REQUIRE(spsl::SensitivePageAllocator::getBitmask(20) == 0b11111111111111111111);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(0) == 0x0);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(1) == 0x1);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(2) == 0x3);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(3) == 0x7);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(4) == 0xf);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(5) == 0x1f);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(6) == 0x3f);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(20) == 0xfffff);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(32) == 0xffffffff);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(63) == all64 >> 1);
    REQUIRE(spsl::SensitivePageAllocator::getBitmask(64) == all64);
}

// allocate a segment
TEST_CASE("ManagedAllocationTest1", "[allocator]")
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);

    // ask for a few bytes
    constexpr std::size_t size1 = 16;
    static_assert(size1 <= spsl::SensitivePageAllocator::segment_size, "oops");
    void* mem = alloc.allocate(size1);
    REQUIRE(mem != nullptr);

    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 1 * alloc.getChunksPerPage());
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);

    alloc.deallocate(mem, size1);

    // in the end, no allocation is left
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
}

using AllocationInfo = spsl::SensitivePageAllocator::AllocationInfo;
using AllocationList = std::vector<AllocationInfo>;

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
        REQUIRE(mem != nullptr);

        allocations.emplace_back(AllocationInfo{ mem, size });

        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 1 * alloc.getChunksPerPage());
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }

    // and now the second one
    for (size_t i = 0; i < alloc.segmentsPerChunk; ++i)
    {
        // ask for a few bytes
        std::size_t size = sizes[i % sizes.size()];
        void* mem = alloc.allocate(size);
        REQUIRE(mem != nullptr);

        allocations.emplace_back(AllocationInfo{ mem, size });

        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 2 * alloc.getChunksPerPage());
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }
}

// allocate and deallocate multiple segments
TEST_CASE("ManagedAllocationTest2", "[allocator]")
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);

    AllocationList allocations;
    performAllocations(alloc, allocations);

    for (auto& entry : allocations)
        alloc.deallocate(entry.addr, entry.size);

    // in the end, no allocation is left
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
}

// allocate multiple segments and release them in reverse order
TEST_CASE("ManagedAllocationTest3", "[allocator]")
{
    spsl::SensitivePageAllocator alloc;

    // nothing allocated yet
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);

    AllocationList allocations;
    performAllocations(alloc, allocations);

    std::reverse(allocations.begin(), allocations.end());
    for (auto& entry : allocations)
        alloc.deallocate(entry.addr, entry.size);

    // in the end, no allocation is left
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
}

// allocate areas large than a page
TEST_CASE("UnmanagedAllocationTest", "[allocator]")
{
    spsl::SensitivePageAllocator alloc;
    auto pageSize = alloc.getPageSize();

    // nothing allocated yet
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);

    // allocating a page is managed
    {
        void* mem = alloc.allocate(pageSize);
        REQUIRE(mem != nullptr);
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 1u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
        alloc.deallocate(mem, pageSize);
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }
    // allocating more is "unmanaged"
    {
        void* mem = alloc.allocate(pageSize + 1);
        REQUIRE(mem != nullptr);
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 1u);
        alloc.deallocate(mem, pageSize + 1);
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }

    // in the end, no allocation is left
    REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 0u);
    REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
}


// test the leak check in the destructor
TEST_CASE("LeakCheckTest1", "[allocator]")
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
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 1u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }

    // now check the leaks
    REQUIRE(myLeaks.size() == cbLeaks.size());
    REQUIRE(cbLeaks.size() == 1u);
    REQUIRE(myInstance == cbInstance);
    REQUIRE(cbLeaks[0].addr == myLeaks[0].addr);
    // these aren't identical because the allocator reserves more space than required
    // REQUIRE(cbLeaks[0].size == myLeaks[0].size);
    REQUIRE(cbLeaks[0].size == 64u);
}


// test the leak check in the destructor even more
TEST_CASE("LeakCheckTest2", "[allocator]")
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
        REQUIRE(allocations.size() == 128u);
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
        REQUIRE(alloc.getNumberOfManagedAllocatedPages() == 2u);
        REQUIRE(alloc.getNumberOfUnmanagedAreas() == 0u);
    }

    // now check the leaks
    REQUIRE(myLeaks.size() > cbLeaks.size());
    REQUIRE(cbLeaks.size() == 6u);
    REQUIRE(myInstance == cbInstance);
    // 0 - 15 - 17 - 65 - 121-123 - 127
    for (std::size_t i = 0; i < 4; ++i)
    {
        REQUIRE(cbLeaks[i].addr == myLeaks[i].addr);
        REQUIRE(cbLeaks[i].size == 64u);
    }
    // 121-123 get "merged"
    REQUIRE(cbLeaks[4].addr == myLeaks[4].addr);
    REQUIRE(cbLeaks[4].size == 3 * 64u);
    REQUIRE(cbLeaks[5].addr == myLeaks[7].addr);
    REQUIRE(cbLeaks[5].size == 64u);
}

// TODO: test other page sizes
