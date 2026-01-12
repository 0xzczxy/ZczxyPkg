#include "Compiler.h"
#include "ExtendedHv.h"

#define CREATE_JUMP(name, targetAddress) \
    unsigned char name[sizeof(JUMP_CODE)]; \
    CopyMem(name, (unsigned char*)JUMP_CODE, sizeof(JUMP_CODE)); \
    *(UINT64*)((UINT64)name + 6) = (UINT64)targetAddress

#define HOOK_R (*b >> 4)
#define HOOK_C (*b & 0xF)

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
// extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallPatch(OUT patchinfo_t *info, IN VOID *originalFunction, IN CONST VOID *targetFunction);
EFI_STATUS UninstallPatch(IN OUT patchinfo_t *info);

// Private Globals
static const unsigned char JUMP_CODE[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static const unsigned char PREFIXES[] = { 0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67 };
static const unsigned char OP1_MODRM[] = { 0x62, 0x63, 0x69, 0x6B, 0xC0, 0xC1, 0xC4, 0xC5, 0xC6, 0xC7, 0xD0, 0xD1, 0xD2, 0xD3, 0xF6, 0xF7, 0xFE, 0xFF };
static const unsigned char OP1_IMM8[] = { 0x6A, 0x6B, 0x80, 0x82, 0x83, 0xA8, 0xC0, 0xC1, 0xC6, 0xCD, 0xD4, 0xD5, 0xEB };
static const unsigned char OP1_IMM32[] = { 0x68, 0x69, 0x81, 0xA9, 0xC7, 0xE8, 0xE9 };
static const unsigned char OP2_MODRM[] = { 0x0D, 0xA3, 0xA4, 0xA5, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF };

// Private Functions
static UINT64 GetInstructionSize(IN CONST VOID *address);
static int FindByte(const unsigned char* buffer, const unsigned long long maxLength, const unsigned char value);
static void ParseModRM(unsigned char** buffer, const int addressPrefix);

// Implementation

EFI_STATUS InstallPatch(OUT patchinfo_t *info, IN VOID *originalFunction, IN CONST VOID *targetFunction) {
  unsigned int size = 0;
  unsigned int iterations = 0;

  if (!info || !originalFunction || !targetFunction)
    return EFI_INVALID_PARAMETER;

  //
  // Disable interrupts and write protection for kernel-mode code modification
  //
  DisableInterrupts();
  DisableMemoryProtection();

  //
  // Calculate how many bytes we need to replace with the jump instruction.
  // Keep adding instruction sizes until we have at least 14 bytes (size of JUMP_CODE).
  // Maximum x86-64 instruction is 15 bytes, so worst case is 15 + 14 = 29 bytes.
  //
  // while (size < sizeof(JUMP_CODE))
  //   size += GetInstructionSize((unsigned char*)originalFunction + size)
  // ;
  SerialPrint("Starting instruction size calculation, need >= %u bytes\n", sizeof(JUMP_CODE));

  while (size < sizeof(JUMP_CODE)) {
    unsigned int instrSize = GetInstructionSize((unsigned char*)originalFunction + size);
    SerialPrint("  Iteration %u: offset=%u, instrSize=%u\n", iterations, size, instrSize);
  
    if (instrSize == 0 || instrSize > 15) {
      SerialPrint("  ERROR: Invalid instruction size!\n");
      return EFI_INVALID_PARAMETER;
    }
  
    size += instrSize;
    iterations++;
  
    if (iterations > 10) {
      SerialPrint("  ERROR: Too many iterations!\n");
      return EFI_INVALID_PARAMETER;
    }
  }

  SerialPrint("Final size: %u bytes in %u iterations\n", size, iterations);

  //
  // Store metadata for later restoration
  //
  info->trampoline = info->buffer; // trampoline is basically an alias (consider fixing this by chaning buffer to be called trampoline)
  info->original_function = originalFunction;
  info->target_function = targetFunction;
  info->size = size;

  //
  // Backup the original bytes we're about to overwrite
  //
  CopyMem(info->buffer, originalFunction, size);

  //
  // Step 1: Create jump from trampoline back to original function (after our patch).
  // This allows the original code to continue executing normally after the trampoline.
  //
  CREATE_JUMP(originalJump, (unsigned char*)originalFunction + size);
  CopyMem(info->buffer + size, originalJump, sizeof(JUMP_CODE));

  //
  // Step 2: Create jump from original function to our hook function.
  // This replaces the original code, so calls to original now call our hook.
  //
  CREATE_JUMP(hookJump, targetFunction);
  CopyMem(originalFunction, hookJump, sizeof(JUMP_CODE));

  //
  // Restore CPU protection mechanisms
  //
  EnableMemoryProtection();
  EnableInterrupts();

  SerialPrint(
    "Patch installed: 0x%p -> 0x%p (trampoline: 0x%p)\n", 
    originalFunction, targetFunction, info->buffer
  );

  return EFI_SUCCESS;
}

EFI_STATUS UninstallPatch(IN OUT patchinfo_t *info) {
  if (!info || !info->original_function)
    return EFI_INVALID_PARAMETER;

  //
  // Disable interrupts and write protection for kernel-mode code modification
  //
  DisableInterrupts();
  DisableMemoryProtection();

  //
  // Restore the original bytes to the original function location
  // This overwrites the jump instruction, removing the hook
  //
  CopyMem(info->original_function, info->buffer, info->size);

  //
  // Clear metadata
  //
  info->original_function = NULL;
  info->target_function = NULL;
  info->size = 0;

  //
  // Restore CPU protection mechanisms
  //
  EnableMemoryProtection();
  EnableInterrupts();

  SerialPrint("Patch uninstalled\n");

  return EFI_SUCCESS;
}

//
// I understand very little of everything below.
// The code is also pretty unreadable even to those who know x86-64 deeper then I.
// 

static UINT64 GetInstructionSize(IN CONST VOID *address) {
  /*
   * Based on length-disassembler by @Nomade040
   * https://github.com/Nomade040/length-disassembler
   */

  unsigned long long offset = 0;
  int operandPrefix = 0, addressPrefix = 0, rexW = 0;
  unsigned char* b = (unsigned char*)address;

  for (int i = 0; (i < 14 && FindByte(PREFIXES, sizeof(PREFIXES), *b)) || HOOK_R == 4; i++, b++) {
    if (*b == 0x66)
      operandPrefix = 1;
    else if (*b == 0x67)
      addressPrefix = 1;
    else if (HOOK_R == 4 && HOOK_C >= 8)
      rexW = 1;
  }

  if (*b == 0x0F) {
    b++;
    if (*b == 0x38 || *b == 0x3A) {
      if (*b++ == 0x3A)
        offset++;
 
      ParseModRM(&b, addressPrefix);
    } else {
      if (HOOK_R == 8)
        offset += 4;
      else if ((HOOK_R == 7 && HOOK_C < 4) || *b == 0xA4 || *b == 0xC2 || (*b > 0xC3 && *b <= 0xC6) || *b == 0xBA || *b == 0xAC)
        offset++;
 
      if (FindByte(OP2_MODRM, sizeof(OP2_MODRM), *b) || (HOOK_R != 3 && HOOK_R > 0 && HOOK_R < 7) || *b >= 0xD0 || (HOOK_R == 7 && HOOK_C != 7) || HOOK_R == 9 || HOOK_R == 0xB || (HOOK_R == 0xC && HOOK_C < 8) || (HOOK_R == 0 && HOOK_C < 4))
        ParseModRM(&b, addressPrefix);
    }
  } else {
    if ((HOOK_R == 0xE && HOOK_C < 8) || (HOOK_R == 0xB && HOOK_C < 8) || HOOK_R == 7 || (HOOK_R < 4 && (HOOK_C == 4 || HOOK_C == 0xC)) || (*b == 0xF6 && !(*(b + 1) & 48)) || FindByte(OP1_IMM8, sizeof(OP1_IMM8), *b))
      offset++;
    else if (*b == 0xC2 || *b == 0xCA)
      offset += 2;
    else if (*b == 0xC8)
      offset += 3;
    else if ((HOOK_R < 4 && (HOOK_C == 5 || HOOK_C == 0xD)) || (HOOK_R == 0xB && HOOK_C >= 8) || (*b == 0xF7 && !(*(b + 1) & 48)) || FindByte(OP1_IMM32, sizeof(OP1_IMM32), *b)) {
      if (*b == 0xB8 || (*b >= 0xB8 && *b <= 0xBF)) // mov r64, imm64
        offset += rexW ? 8 : 4;
      else if (*b == 0xC7) // mov r/m64, imm32 (sign-extended to 64-bits)
        offset += 4;
      else if (*b == 0x69) // imul r64, r/m64, imm32
        offset += 4;
      else
        offset += (operandPrefix) ? 2 : 4;
    }
    else if (HOOK_R == 0xA && HOOK_C < 4)
      offset += (rexW) ? 8 : (addressPrefix ? 2 : 4);
    else if (*b == 0xEA || *b == 0x9A)
      offset += operandPrefix ? 4 : 6;
 
    if (FindByte(OP1_MODRM, sizeof(OP1_MODRM), *b) || (HOOK_R < 4 && (HOOK_C < 4 || (HOOK_C >= 8 && HOOK_C < 0xC))) || HOOK_R == 8 || (HOOK_R == 0xD && HOOK_C >= 8))
      ParseModRM(&b, addressPrefix);
  }

  return (UINT64)(++b + offset - (unsigned char*)address);
}

static int FindByte(const unsigned char* buffer, const unsigned long long maxLength, const unsigned char value) {
  for (unsigned long long i = 0; i < maxLength; i++) {
    if (buffer[i] == value)
      return 1;
  }

  return 0;
}

static void ParseModRM(unsigned char** buffer, const int addressPrefix) {
  const unsigned char modRm = *++ * buffer;

  if (!addressPrefix || (addressPrefix && **buffer >= 0x40)) {
    int hasSib = 0;
    if (**buffer < 0xC0 && (**buffer & 0b111) == 0b100 && !addressPrefix)
      hasSib = 1, (*buffer)++;

    if (modRm >= 0x40 && modRm <= 0x7F)
      (*buffer)++;
    else if ((modRm <= 0x3F && (modRm & 0b111) == 0b101) || (modRm >= 0x80 && modRm <= 0xBF))
      *buffer += (addressPrefix) ? 2 : 4;
    else if (hasSib && (**buffer & 0b111) == 0b101)
      *buffer += (modRm & 0b01000000) ? 1 : 4;
  } else if (addressPrefix && modRm == 0x26)
      *buffer += 2;
}

