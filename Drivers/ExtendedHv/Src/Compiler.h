#ifndef COMPILER_INTRINSICS_H
#define COMPILER_INTRINSICS_H

#if defined(_MSC_VER)
    /* MSVC Intrinsics */
    #include <intrin.h>

    static inline UINT64 GetReturnAddress(VOID)
    {
        return (UINT64)_ReturnAddress();
    }

    static inline VOID EnableMemoryProtection(VOID)
    {
        UINT64 cr0 = __readcr0();
        cr0 |= (1ULL << 16);  /* Set WP (Write Protect) bit */
        __writecr0(cr0);
    }

    static inline VOID DisableMemoryProtection(VOID)
    {
        UINT64 cr0 = __readcr0();
        cr0 &= ~(1ULL << 16);  /* Clear WP (Write Protect) bit */
        __writecr0(cr0);
    }

    static inline VOID FlushInstructionCache(VOID *address, UINTN size)
    {
        // Serialize instruction execution
        __cpuid((int*)address, 0);
    }

#elif defined(__GNUC__)
    /* GCC Inline Assembly */

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
        __asm__ volatile(
            "mfence\n\t"           // Memory fence
            "cpuid\n\t"            // Serializing instruction
            :
            :
            : "rax", "rbx", "rcx", "rdx", "memory"
        );
    }

#else
    #error "Unsupported compiler. Please use MSVC or GCC."
#endif

#endif /* COMPILER_INTRINSICS_H */
