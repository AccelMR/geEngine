/*****************************************************************************/
/**
 * @file    geFrameAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Frame allocator
 *
 * Performs very fast allocations but can only free all of its memory at once.
 * Perfect for allocations that last just a single frame.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include <limits>
#include <new>

#include "gePlatformDefines.h"
#include "gePlatformTypes.h"
#include "geStdHeaders.h"
#include "geThreadDefines.h"

namespace geEngineSDK {
  /**
   * @brief Frame allocator. Performs very fast allocations but can only free all of its
   *        memory at once. Perfect for allocations that last just a single frame.
   * @note  Not thread safe with an exception. alloc() and clear() methods need to be called
   *        from the same thread. Dealloc() is thread safe and can be called from any thread.
   */
  class GE_UTILITY_EXPORT FrameAlloc
  {
   private:
    /**
     * @brief A single block of memory within a frame allocator.
     */
    class MemBlock
    {
     public:
      explicit MemBlock(SIZE_T size);
      ~MemBlock();

      /**
       * @brief Allocates a piece of memory within the block. Caller must
       *        ensure the block has enough empty space.
       */
      uint8*
      alloc(SIZE_T amount);

      /**
       * @brief Releases all allocations within a block but doesn't actually free the memory.
       */
      void
      clear();

     public:
      uint8* m_data;
      SIZE_T m_freePtr;
      SIZE_T m_size;
    };

   public:
    explicit FrameAlloc(SIZE_T blockSize = 1024 * 1024);
    ~FrameAlloc();

    /**
     * @brief Allocates a new block of memory of the specified size.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @note  Not thread safe.
     */
    uint8*
    alloc(SIZE_T amount);

    /**
     * @brief Allocates a new block of memory of the specified size aligned to
     *        the specified boundary. If the alignment is less or equal to 16 it is
     *        more efficient to use the allocAligned16() alternative of this method.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @param[in] alignment Alignment of the allocated memory. Must be power of two.
     * @note  Not thread safe.
     */
    uint8*
    allocAligned(SIZE_T amount, SIZE_T alignment);

    /**
     * @brief Allocates and constructs a new object.
     * @note  Not thread safe.
     */
    template<class T, class... Args>
		T*
    construct(Args &&...args) {
			return new ((T*)alloc(sizeof(T))) T(std::forward<Args>(args)...);
		}

    /**
     * @brief Deallocates a previously allocated block of memory.
     * @note  No deallocation is actually done here. This method is only used
     *        for debug purposes so it is easier to track down memory leaks and
     *        corruption.
     * @note  Thread safe.
     */
    void
    free(uint8* data);

    /**
     * @brief Deallocates and destructs a previously allocated object.
     * @note  No deallocation is actually done here. This method is only used
     *        to call the destructor and for debug purposes so it is easier to 
     *        track down memory leaks and corruption.
     * @note  Thread safe.
     */
    template<class T>
    void
    free(T* obj) {
      if (nullptr != obj) {
        obj->~T();
      }
      free(reinterpret_cast<uint8*>(obj));
    }

    /**
     * @brief Starts a new frame. Next call to ::Clear will only clear memory
     *        allocated past this point.
     */
    void
    markFrame();

    /**
     * @brief Deallocates all allocated memory since the last call to markFrame()
     *        (or all the memory if there was no call to markFrame()).
     * @note  Not thread safe.
     */
    void
    clear();

    /**
     * @brief Changes the frame allocator owner thread. After the owner thread
     *        has changed only allocations from that thread can be made.
     */
    void
    setOwnerThread(ThreadId thread);

   private:
    /**
     * @brief Allocates a dynamic block of memory of the wanted size. The exact
     *        allocation size might be slightly higher in order to store block
     *        meta data.
     */
    MemBlock*
    allocBlock(SIZE_T wantedSize);

    /**
    * @brief Frees a memory block.
    */
    void
    deallocBlock(MemBlock* block);

   private:
    SIZE_T m_blockSize;
    Vector<MemBlock*> m_blocks;
    MemBlock* m_freeBlock;
    uint32 m_nextBlockIdx;
    std::atomic<SIZE_T> m_totalAllocBytes;
    void* m_lastFrame;

#if GE_DEBUG_MODE
    ThreadId m_ownerThread;
#endif
  };

  /**
   * @brief Version of FrameAlloc that allows blocks size to be provided
   *        through the template argument instead of the constructor.
   */
  template<int BlockSize>
  class TFrameAlloc : public FrameAlloc
  {
   public:
    TFrameAlloc() : FrameAlloc(BlockSize) {}
  };

  /**
   * @brief Allocator for the standard library that internally uses a frame
   *        allocator.
   */
  template <class T>
  class StdFrameAlloc
  {
   public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
    StdFrameAlloc() _NOEXCEPT : m_FrameAlloc(nullptr) {}
    StdFrameAlloc(FrameAlloc* pAlloc) _NOEXCEPT : m_FrameAlloc(pAlloc) {}

    template<class U>
    StdFrameAlloc(const StdFrameAlloc<U>& refAlloc) _NOEXCEPT
      : m_FrameAlloc(refAlloc.m_FrameAlloc) {
    }

    template<class U>
    bool
    operator==(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return true;
    }

    template<class U>
    bool
    operator!=(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return false;
    }

    template<class U>
    class rebind
    {
     public: typedef StdFrameAlloc<U> other;
    };

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    T*
    allocate(const size_t num) const {
      if (0 == num) {
        return nullptr;
      }

      if (num > (static_cast<size_t>(-1) / sizeof(T))){
        return nullptr; //Error
      }

      void* const pv = m_FrameAlloc->alloc(num * sizeof(T));
      if (!pv) {
        return nullptr; //Error
      }

      return static_cast<T*>(pv);
    }

    /**
     * @brief Deallocate storage p of deleted elements.
     */
    void
    deallocate(T* p, size_t) const _NOEXCEPT {
      m_FrameAlloc->free(reinterpret_cast<uint8*>(p));
    }

   public:
    FrameAlloc* m_FrameAlloc;

    size_t
    max_size() const {
      return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    void
    construct(pointer p, const_reference t) {
      new (p) T(t);
    }
    
    void
    destroy(pointer p) {
      p->~T();
    }

    template<class U, class... Args>
    void
    construct(U* p, Args&& ...args) {
      new(p) U(std::forward<Args>(args)...);
    }
  };

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator==(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return true;
  }

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator!=(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return false;
  }

  /**
   * @brief Returns a global, application wide FrameAlloc. Each thread gets its
   *        own frame allocator.
   * @note  Thread safe.
   */
  GE_UTILITY_EXPORT FrameAlloc&
  g_frameAlloc();

  /**
   * @brief Allocates some memory using the global frame allocator.
   * @param[in] numBytes  Number of bytes to allocate.
   */
  GE_UTILITY_EXPORT uint8*
  ge_frame_alloc(SIZE_T numBytes);

  /**
   * @brief Allocates the specified number of bytes aligned to the provided
   *        boundary, using the global frame allocator. Boundary is in bytes
   *        and must be a power of two.
   */
  GE_UTILITY_EXPORT uint8*
  ge_frame_alloc_aligned(SIZE_T count, SIZE_T align);

  /**
   * @brief Deallocates memory allocated with the global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITY_EXPORT void
  ge_frame_free(void* data);

  /**
   * @brief Frees memory previously allocated with ge_frame_alloc_aligned().
   * @note  Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITY_EXPORT void
  ge_frame_free_aligned(void* data);

  /**
   * @brief Allocates enough memory to hold the object of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc() {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T)));
  }

  /**
   * @brief Allocates enough memory to hold N objects of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc(SIZE_T count) {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T) * count));
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T>
  T*
  ge_frame_new(SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);

    for (SIZE_T i = 0; i < count; ++i) {
      new ((void*)&data[i]) T;
    }
    return data;
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T, class... Args>
  T*
  ge_frame_new(Args &&...args, SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);

    for (SIZE_T i = 0; i < count; ++i) {
      new ((void*)&data[i]) T(std::forward<Args>(args)...);
    }
    return data;
  }

  /**
   * @brief Destructs and deallocates an object allocated with the global frame
   *        allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data) {
    data->~T();
    ge_frame_free(reinterpret_cast<uint8*>(data));
  }

  /**
   * @brief Destructs and deallocates an array of objects allocated with the
   *        global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data, SIZE_T count) {
    for (SIZE_T i = 0; i < count; ++i) {
      data[i].~T();
    }
    ge_frame_free(reinterpret_cast<uint8*>(data));
  }

  /**
   * @copydoc FrameAlloc::markFrame
   */
  GE_UTILITY_EXPORT void
  ge_frame_mark();

  /**
   * @copydoc FrameAlloc::clear
   */
  GE_UTILITY_EXPORT void
  ge_frame_clear();

  /**
   * @brief String allocated with a frame allocator.
   */
  typedef std::basic_string<ANSICHAR,
                            std::char_traits<ANSICHAR>,
                            StdAlloc<ANSICHAR, FrameAlloc>>
                            FrameString;

  /**
   * @brief WString allocated with a frame allocator.
   */
  typedef std::basic_string<UNICHAR,
                            std::char_traits<UNICHAR>,
                            StdAlloc<UNICHAR, FrameAlloc>>
                            FrameWString;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameVector = std::vector<T, A>;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameStack = std::stack<T, std::deque<T, A>>;

  template <typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameQueue = std::queue<T, std::deque<T, A>>;

  template<typename T, typename P = std::less<T>, typename A = StdAlloc<T, FrameAlloc>>
  using FrameSet = std::set<T, P, A>;

  template<typename K, 
           typename V,
           typename P = std::less<K>,
           typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
  using FrameMap = std::map<K, V, P, A>;

  template<typename T,
           typename H = std::hash<T>,
           typename C = std::equal_to<T>,
           typename A = StdAlloc<T, FrameAlloc>>
  using FrameUnorderedSet = std::unordered_set<T, H, C, A>;

  template<typename K,
           typename V,
           typename H = std::hash<K>,
           typename C = std::equal_to<K>,
           typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
  using FrameUnorderedMap = std::unordered_map<K, V, H, C, A>;

  extern GE_THREADLOCAL FrameAlloc* _globalFrameAlloc;

  /**
   * @brief Specialized memory allocator implementations that allows use of a
   *        global frame allocator in normal new/delete/free/dealloc operators.
   */
  template<>
  class MemoryAllocator<FrameAlloc> : public MemoryAllocatorBase
  {
  public:
    /**
     * @copydoc MemoryAllocator::allocate
     */
    static void*
    allocate(SIZE_T bytes) {
      return ge_frame_alloc(bytes);
    }

    /**
     * @copydoc MemoryAllocator::allocateAligned
     */
    static void*
    allocateAligned(SIZE_T bytes, SIZE_T alignment) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, alignment);
    }

    /**
     * @copydoc MemoryAllocator::allocateAligned16
     */
    static void*
    allocateAligned16(SIZE_T bytes) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, 16);
    }

    /**
     * @copydoc MemoryAllocator::free
     */
    static void
    free(void* ptr) {
      ge_frame_free(ptr);
    }

    /**
     * @copydoc MemoryAllocator::freeAligned
     */
    static void
    freeAligned(void* ptr) {
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }

    /**
     * @copydoc MemoryAllocator::freeAligned16
     */
    static void
    freeAligned16(void* ptr) {
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }
  };
}
