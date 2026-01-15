//
// Compile with mingw
//

#include <stdint.h>
#include <stdio.h>

int main() {
  uint64_t leaf = 0xDEADBEEFDEADBEEFull;
  uint64_t output = 0x123;

  __asm__ volatile (
    "cpuid\n\t"
    "mov %%r8, %0"
    : "=r"(output)
    : "a" (leaf)
    : "rbx", "rcx", "rdx"
  );

  printf("R8 After CPUID = 0x%016lx\n", output);

  return 0;
}

