/**
 * @file    Special Purpose Strings Library: pagealloc.hpp
 * @author  Daniel Evers
 * @brief   Page allocator implementation
 * @license MIT
 */

#ifndef SPSL_PAGEALLOC_HPP_
#define SPSL_PAGEALLOC_HPP_

#include <algorithm>
#include <cstdio> // less overhead than iostreams...
#include <functional>
#include <mutex>
#include <vector>

#include "spsl/compat.hpp"


namespace spsl
{

/**
 * This class allocates full pages, marks them as "do not swap" and "do not dump".
 *
 * The page size is system-dependent, but usually 4K. To handle larger pages, they are divided
 * into "chunks" of 4K size (so usually a 1:1 relationship), thus requiring that the page size
 * must be a multiple of 4K.
 * Each of these 4K chunks is divided into 64 "segments" of 64K that can be reserved and passed
 * to the application. This requires 64 bit of "management" info for each chunk to keep track of
 * free and reserved pages.
 *
 * The allocator will always allocate one or more pages at once, depending on the required space.
 * If an application tries to allocate more than one page, multiple contiguous pages are
 * allocated.
 * Due to this allocation strategy, this class should be used like a singleton. Multiple instances
 * are possible, but fewer instances result in less wasted memory.
 * To use only a single instance, use @see getDefaultInstance().
 *
 * Final note: The internal bitmask assumes a little endian system...
 */
class SensitivePageAllocator
{
public:
    /// memory is reserved in segments of 64 bytes
    static constexpr std::size_t segment_size = 64;
    /// pages are split in chunks of 4K
    static constexpr std::size_t chunk_size = 4096;
    /// number of segments per chunk
    static constexpr std::size_t segmentsPerChunk = chunk_size / segment_size;

    static constexpr uint64_t all64 = 0xffffffffffffffff;

    using pointer = void*;

    /// information about an allocated area of memory
    struct AllocationInfo
    {
        /// the allocated address
        pointer addr;
        /// number of bytes allocated
        std::size_t size;
    };

    /// Callback function type (see below)
    using LeakCallbackFunction =
      std::function<void(const SensitivePageAllocator*, const AllocationInfo&, bool)>;

    /**
     * Constructor: Initializes the allocator, but doesn't yet allocate anything.
     * @param[in] pageSize      the OS's page size (or a multiple thereof)
     * @throws std::runtime_error  if the pageSize isn't a multiple of the segment or the chunk size
     */
    explicit SensitivePageAllocator(std::size_t pageSize = os::getPageSize())
      : m_mutex(), m_pageSize(pageSize), m_chunksPerPage(m_pageSize / chunk_size),
        m_managedChunks(), m_unmanagedAreas(), m_leakCallback(logLeaks)
    {
        // the page size is expected to be a multiple of the segment size
        if (m_pageSize % segment_size != 0)
            throw std::runtime_error("expected the page size to be a multiple of the segment size");

        // ... and of 4096
        if (m_pageSize % chunk_size != 0)
            throw std::runtime_error("expected the page size to be a multiple of the chunk size");

        static_assert(chunk_size % segment_size == 0,
                      "The segment size must be a multiple of the chunk size");

        // reserve some memory upfront
        m_managedChunks.reserve(16);
        m_unmanagedAreas.reserve(16);
    }

    /**
     * Destructor: Doesn't actually need to do anything, but checks if there are any segments
     * or unmanaged areas that are still in use. If so, the leak callback is called for every
     * location.
     */
    ~SensitivePageAllocator()
    {
        // check for memory that is still in use
        bool firstCbCall = true;

        // check each chunk and tell the callback
        if (m_leakCallback)
        {
            for (auto& chunk : m_managedChunks)
            {
                if (chunk.segments != all64)
                {
                    // call the callback function for every contiguous range of addresses
                    while (chunk.segments != all64)
                    {
                        // search for the first used segment
                        uint64_t mask = 0x1;
                        uint64_t resetMask = 0;
                        std::size_t startIndex = 0;
                        std::size_t endIndex = 0;
                        bool searchingForStart = true;
                        for (std::size_t i = 0; i < 64; ++i)
                        {
                            if (searchingForStart)
                            {
                                if ((chunk.segments & mask) == 0)
                                {
                                    startIndex = i;
                                    endIndex = 63;
                                    searchingForStart = false;
                                }
                            }
                            else if ((chunk.segments & mask) == mask)
                            {
                                endIndex = i - 1;
                                break;
                            }
                            resetMask |= mask;
                            mask <<= 1;
                        }

                        // mask as free
                        chunk.segments |= resetMask;

                        // notify the callback
                        m_leakCallback(this,
                                       AllocationInfo{ reinterpret_cast<char*>(chunk.addr) +
                                                         startIndex * segment_size,
                                                       (endIndex - startIndex + 1) * segment_size },
                                       firstCbCall);
                        firstCbCall = false;
                    }
                }
            }
        }
        // finally, remove all associated pages
        while (!m_managedChunks.empty())
        {
            auto pageAddr = m_managedChunks.front().pageAddr;
            deallocatePage(pageAddr, m_pageSize);

            m_managedChunks.erase(std::remove_if(m_managedChunks.begin(), m_managedChunks.end(),
                                                 [pageAddr](const ChunkManagementInfo& chunk) {
                                                     return chunk.pageAddr == pageAddr;
                                                 }),
                                  m_managedChunks.end());
        }

        // and now the "unmanaged" pages
        for (auto& area : m_unmanagedAreas)
        {
            if (m_leakCallback)
            {
                m_leakCallback(this, area, firstCbCall);
                firstCbCall = false;
            }
            deallocatePage(area.addr, area.size);
        }
    }

    // disable copy & move
    SensitivePageAllocator(const SensitivePageAllocator&) = delete;
    SensitivePageAllocator(SensitivePageAllocator&&) = delete;
    SensitivePageAllocator& operator=(const SensitivePageAllocator&) = delete;
    SensitivePageAllocator& operator=(SensitivePageAllocator&&) = delete;

    /**
     * Returns the "default instance", a.k.a. a static instance of the allocator. The instance is
     * created upon first use and destroyed when the application exits.
     * Note: This function may throw any exception that the constructor might throw.
     * @return a reference to the instance
     */
    static SensitivePageAllocator& getDefaultInstance()
    {
        // "phoenix singleton"
        static SensitivePageAllocator _instance;
        return _instance;
    }

    /**
     * Sets the leak callback function. This function is called by the destructor for every memory
     * location that hasn't been deallocated yet.
     *
     * Note: This method is intentionally *not* thread-safe.
     * Another note: Pass @c nullptr to disable leak checks.
     *
     * @param[in] fun       the function to call
     */
    void setLeakCallback(LeakCallbackFunction fun) { m_leakCallback = std::move(fun); }


    /**
     * Default leak callback: logs to stderr.
     * @param[in] instance      the PageAllocator instance
     * @param[in] leak          information about the area that hasn't been deallocated
     * @param[in] first         set to @c true for the first call of this function
     */
    static void logLeaks(const SensitivePageAllocator* instance, const AllocationInfo& leak,
                         bool first)
    {
        if (first)
        {
            const void* ptr = instance;
            fprintf(stderr, "!!! Leaks detected in PageAllocator(%p):\n", ptr);
        }
        fprintf(stderr, "!!! %zu bytes @ address %p\n", leak.size, leak.addr);
    }

    std::size_t max_size() const noexcept { return static_cast<std::size_t>(-1); }

    std::size_t getPageSize() const { return m_pageSize; }
    std::size_t getChunksPerPage() const { return m_chunksPerPage; }

    static inline constexpr std::size_t calcSegmentCount(std::size_t n)
    {
        // "round" n up to a multiple of the segment size
        return (((n - 1) / segment_size) + 1);
    }
    static inline constexpr std::size_t roundToSegmentSize(std::size_t n)
    {
        return calcSegmentCount(n) * segment_size;
    }

    inline std::size_t calcPageCount(std::size_t n)
    {
        // "round" n up to a multiple of the page size
        return (((n - 1) / m_pageSize) + 1);
    }
    inline std::size_t roundToPageSize(std::size_t n) { return calcPageCount(n) * m_pageSize; }

    static inline constexpr uint64_t getBitmask(std::size_t n)
    {
        return (n == segmentsPerChunk ? all64 : ((static_cast<uint64_t>(1) << n) - 1));
    }


    // Some informal stuff...

    std::size_t getNumberOfManagedAllocatedPages()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_managedChunks.size() / m_chunksPerPage;
    }

    std::size_t getNumberOfUnmanagedAreas()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_unmanagedAreas.size();
    }


    /**
     * Allocates a range of memory.
     * @param[in] size      the minimum size of the allocation
     * @return memory address
     * @throws std::bad_alloc if allocation failed
     * @throws std::system_error if locking fails
     */
    pointer allocate(std::size_t size)
    {
        std::size_t n = calcSegmentCount(size);
        if (n <= segmentsPerChunk)
        {
            return allocateSegment(n);
        }
        else
        {
            return allocateUnmanaged(size);
        }
    }

    /**
     * Deallocates a range of memory.
     * @param[in] addr      the memory address that was returned from allocate()
     * @param[in] size      the parameter previously passed to allocate()
     * @throws std::system_error if unlocking fails
     */
    void deallocate(pointer addr, std::size_t size)
    {
        std::size_t n = calcSegmentCount(size);
        if (n <= segmentsPerChunk)
        {
            deallocateSegment(addr, n);
        }
        else
        {
            deallocateUnmanaged(addr, size);
        }
    }

private:
    /**
     * Allocates a range of memory that isn't managed in segments, but provided to the caller
     * completely.
     * @param[in] size      the required size
     * @return the allocate memory
     * @throws std::bad_alloc if allocation failed
     * @throws std::system_error if locking fails
     */
    pointer allocateUnmanaged(std::size_t size)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        size = roundToPageSize(size);

        // reserve memory for the management info upfront - if it throws, we don't have to clean up
        m_unmanagedAreas.reserve(m_unmanagedAreas.size() + 1);

        pointer addr = allocatePage(m_pageSize, size);

        // save
        m_unmanagedAreas.emplace_back(AllocationInfo{ addr, size });
        return addr;
    }

    /**
     * Deallocates memory.
     * @param[in] addr      the address previously returned by allocateUnmanaged()
     * @param[in] size      the size of the allocation
     * @throws std::system_error if unlocking fails
     */
    void deallocateUnmanaged(pointer addr, std::size_t size)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        size = roundToPageSize(size);
        deallocatePage(addr, size);
        m_unmanagedAreas.erase(
          std::remove_if(m_unmanagedAreas.begin(), m_unmanagedAreas.end(),
                         [=](const AllocationInfo& a) { return a.addr == addr; }),
          m_unmanagedAreas.end());
    }

    /**
     * Allocates a contiguous range of n segments.
     * @param[in] n     the number of segments to reserver
     * @return the allocate memory
     * @throws std::bad_alloc if allocation failed
     * @throws std::system_error if locking fails
     */
    pointer allocateSegment(std::size_t n)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // turn 'n' into a mask of n bits (while avoiding overflow)
        const uint64_t bitmask = getBitmask(n);

        // check all chunks we have allocated
        for (auto& chunk : m_managedChunks)
        {
            // bits: 1 = free, 0 = reserved
            // search from index 0 to the last possible (size - n)
            uint64_t mask = bitmask;
            for (std::size_t index = 0; index <= segmentsPerChunk - n; ++index)
            {
                if ((chunk.segments & mask) == mask)
                {
                    // found! -> mark as reserved
                    chunk.segments &= ~mask;
                    return reinterpret_cast<char*>(chunk.addr) + index * segment_size;
                }
                // shift the mask
                mask <<= 1;
            }
        }

        // reserve memory for the management info upfront - if it throws, we don't have to clean up
        m_managedChunks.reserve(m_managedChunks.size() + m_chunksPerPage);

        // nothing found yet -> need to allocate a new page
        pointer addr = allocatePage(m_pageSize, m_pageSize);

        // add the chunks
        for (std::size_t i = 0; i < m_chunksPerPage; ++i)
        {
            m_managedChunks.push_back(
              ChunkManagementInfo{ reinterpret_cast<char*>(addr) + i * chunk_size, addr, all64 });
        }
        ChunkManagementInfo& chunk = m_managedChunks[m_managedChunks.size() - m_chunksPerPage];

        // mark the first N segments as reserved
        chunk.segments &= ~bitmask;

        return chunk.addr;
    }

    /**
     * Deallocates memory.
     * @param[in] addr      the address previously returned by allocateSegment()
     * @param[in] n         the number of segments to release
     * @throws std::system_error if unlocking fails
     */
    void deallocateSegment(pointer addr, std::size_t n)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // turn 'n' into a mask of n bits (while avoiding overflow)
        const uint64_t bitmask = getBitmask(n);

        // find the matching chunk and save the page's address
        pointer pageAddr = nullptr;
        for (auto& chunk : m_managedChunks)
        {
            if (addr >= chunk.addr && addr < reinterpret_cast<char*>(chunk.addr) + chunk_size)
            {
                // found: calculate the index and the bit mask
                std::size_t index = static_cast<std::size_t>(reinterpret_cast<char*>(addr) -
                                                             reinterpret_cast<char*>(chunk.addr)) /
                                    segment_size;
                uint64_t mask = bitmask << index;
                chunk.segments |= mask;

                // all free?
                if (chunk.segments == all64)
                {
                    // save the page's address (to cleanup?)
                    pageAddr = chunk.pageAddr;
                }
                break;
            }
        }

        if (pageAddr)
        {
            // can the whole page be released?
            for (auto& chunk : m_managedChunks)
            {
                if (chunk.pageAddr == pageAddr)
                {
                    if (chunk.segments != all64)
                        // something is still in use
                        return;
                }
            }
            deallocatePage(pageAddr, m_pageSize);
            m_managedChunks.erase(std::remove_if(m_managedChunks.begin(), m_managedChunks.end(),
                                                 [pageAddr](const ChunkManagementInfo& chunk) {
                                                     return chunk.pageAddr == pageAddr;
                                                 }),
                                  m_managedChunks.end());
        }
    }

    static pointer allocatePage(std::size_t pageSize, std::size_t size)
    {
        pointer addr = os::allocatePageAligned(pageSize, size);
        if (!addr)
            throw std::bad_alloc();

        std::error_code ec;
        os::lockMemory(addr, size, &ec);
        if (ec)
        {
            fprintf(stderr, "Failed to lock memory page: %s\n", ec.message().c_str());
            ec.clear();
        }
        os::disableDump(addr, size, &ec);
        if (ec)
            fprintf(stderr, "Failed to disable core dump: %s\n", ec.message().c_str());
        return addr;
    }

    static void deallocatePage(pointer addr, std::size_t size) noexcept
    {
        std::error_code ec;
        os::unlockMemory(addr, size, &ec);
        if (ec)
        {
            fprintf(stderr, "Failed to unlock memory page: %s\n", ec.message().c_str());
            ec.clear();
        }
        os::enableDump(addr, size, &ec);
        if (ec)
            fprintf(stderr, "Failed to re-enable core dump: %s\n", ec.message().c_str());
        os::deallocatePageAligned(addr);
    }

private:
    /// Information about a chunk that is managed
    struct ChunkManagementInfo
    {
        /// points to the beginning of the chunk
        pointer addr;
        /// points to the beginning of the page - if != addr, this chunk cannot be deallocated
        /// itself
        pointer pageAddr;
        /// status of all segments in this chunk
        uint64_t segments;
    };

    /// mutex to ensure allocation across threads
    std::mutex m_mutex;

    /// the system's page size - usually 4K
    std::size_t m_pageSize;
    /// number of chunks per page (usually 1)
    std::size_t m_chunksPerPage;

    /// the areas of memory we allocated and manage in segments and chunks
    std::vector<ChunkManagementInfo> m_managedChunks;
    /// allocated memory that isn't managed in segments, because they are larger than a chunk
    std::vector<AllocationInfo> m_unmanagedAreas;

    /// This function is called by the destructor for every memory location that hasn't been
    /// deallocated yet. The default implementation prints to stderr.
    LeakCallbackFunction m_leakCallback;
};


/**
 * This adapter template is intended to be used with STL templates or the string templates used
 * in the SPSL. Since the PageAllocator class is intended to be used as a singleton (but doesn't
 * has to be), using it directly in STL templates may waste a lot of memory.
 */
template <typename T>
class SensitiveSegmentAllocator
{
public:
    using value_type = T;
    using propagate_on_container_move_assignment = std::true_type;

    // constructors
    SensitiveSegmentAllocator() : m_alloc(&SensitivePageAllocator::getDefaultInstance()) {}
    SensitiveSegmentAllocator(SensitivePageAllocator& alloc) noexcept : m_alloc(&alloc) {}
    SensitiveSegmentAllocator(const SensitiveSegmentAllocator& other) noexcept = default;
    SensitiveSegmentAllocator(SensitiveSegmentAllocator&& other) noexcept : m_alloc(nullptr)
    {
        this->swap(other);
    }
    template <class U>
    explicit SensitiveSegmentAllocator(const SensitiveSegmentAllocator<U>& other) noexcept
      : m_alloc(other.m_alloc)
    {
    }

    SensitiveSegmentAllocator& operator=(const SensitiveSegmentAllocator& other) noexcept = default;
    SensitiveSegmentAllocator& operator=(SensitiveSegmentAllocator&& other) noexcept
    {
        this->swap(other);
        return *this;
    }

    // default destructor
    ~SensitiveSegmentAllocator() = default;

    /**
     * Swaps this allocator with another one by swapping the referenced page allocator.
     * @param[in] other    the allocator to swap with
     */
    void swap(SensitiveSegmentAllocator& other) noexcept { std::swap(m_alloc, other.m_alloc); }

    [[nodiscard]] SensitivePageAllocator* pageAllocator() const noexcept { return m_alloc; }

    // allocation / deallocation

    T* allocate(std::size_t n) { return static_cast<T*>(m_alloc->allocate(n * sizeof(T))); }

    void deallocate(T* p, std::size_t n) { m_alloc->deallocate(p, n * sizeof(T)); }

    std::size_t max_size() const noexcept { return m_alloc->max_size() / sizeof(T); }

private:
    /// non-owning pointer to the "real" allocator
    /// (nullptr is only possible in a moved-from state)
    SensitivePageAllocator* m_alloc;
};
} // namespace spsl


#endif /* SPSL_PAGEALLOC_HPP_ */
