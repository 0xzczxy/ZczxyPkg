//
// COMMON DEFINITIONS FOR HVIX64.EXE
//

#include <stdint.h>

typedef struct {
  uint64_t _upper;
  uint64_t _lower;
} uint128_t;

//
// Reference "https://github.com/backengineering/Voyager/blob/master/Voyager/PayLoad%20(Intel)/types.h"
//
// This structure is likely to be outdated with all the new additions to modern CPUs,
// however, I do not think the first registers will change, only new additions should be
// added, hopefully, windows 11 does not break backwards compatibility for their own
// and our sake.
//
struct context {
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbx;
	uint64_t rsp;
	uint64_t rbp;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint128_t xmm0;
	uint128_t xmm1;
	uint128_t xmm2;
	uint128_t xmm3;
	uint128_t xmm4;
	uint128_t xmm5;
};
typedef struct context context_t;


