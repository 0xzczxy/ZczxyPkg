#ifndef __COMPILER_INTRINSICS_H
#define __COMPILER_INTRINSICS_H

#if defined(_MSC_VER)
  /* MSVC Intrinsics */
  #include <intrin.h>

  #define STRUCTURE_ALLIGN(size) __declspec(align((size)))

  static inline UINT64 GetReturnAddress(VOID)
  {
    return (UINT64)_ReturnAddress();
  }

  static inline VOID EnableMemoryProtection(VOID)
  {
    UINT64 cr0 = __readcr0();
    cr0 |= (1ULL << 16); /* Set WP (Write Protect) bit */
    __writecr0(cr0);
  }

  static inline VOID DisableMemoryProtection(VOID)
  {
    UINT64 cr0 = __readcr0();
    cr0 &= ~(1ULL << 16); /* Clear WP (Write Protect) bit */
    __writecr0(cr0);
  }

  static inline VOID FlushInstructionCache(VOID *address, UINTN size)
  {
    int cpuInfo[4];
    UINTN offset;
    
    // Flush cache lines (64-byte aligned)
    for (offset = 0; offset < size; offset += 64) {
      _mm_clflush((void*)((UINT64)address + offset));
    }
    
    // Memory fence and serialization
    _mm_mfence();
    __cpuid(cpuInfo, 0);
  }

#elif defined(__GNUC__)
  /* GCC Inline Assembly */

  #define STRUCTURE_ALLIGN(size) __attribute__((aligned((size))))

  static inline UINT64 GetReturnAddress(VOID)
  {
    return (UINT64)__builtin_return_address(0);
  }

  static inline VOID EnableMemoryProtection(VOID)
  {
    UINT64 cr0;
    __asm__ volatile(
      "movq %%cr0, %0\n\t"
      "orq %1, %0\n\t"
      "movq %0, %%cr0"
      : "=&r" (cr0)
      : "i" (1ULL << 16)
      : "memory"
    );
  }

  static inline VOID DisableMemoryProtection(VOID)
  {
    UINT64 cr0;
    UINT64 mask = ~(1ULL << 16);
    __asm__ volatile(
      "movq %%cr0, %0\n\t"
      "andq %1, %0\n\t"
      "movq %0, %%cr0"
      : "=&r" (cr0)
      : "r" (mask)
      : "memory"
    );
  }

  static inline VOID FlushInstructionCache(VOID *address, UINTN size)
  {
    UINT64 addr = (UINT64)address;
    UINT64 end = addr + size;
    
    // Align to cache line boundary (64 bytes)
    addr &= ~63ULL;
    
    // Flush all cache lines covering the range
    for (; addr < end; addr += 64) {
      __asm__ volatile(
        "clflush (%0)\n\t"
        :
        : "r" (addr)
        : "memory"
      );
    }
    
    // Memory fence and serialize instruction execution
    __asm__ volatile(
      "mfence\n\t"
      "cpuid\n\t"
      :
      :
      : "rax", "rbx", "rcx", "rdx", "memory"
    );
  }

#else
  #error "Unsupported compiler. Please use MSVC or GCC."
#endif

#endif // __COMPILER_INTRINSICS_H
