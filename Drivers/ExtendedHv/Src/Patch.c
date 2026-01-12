#include "Compiler.h"
#include "ExtendHv.h"

#define CREATE_JUMP(name, targetAddress) \
    unsigned char name[sizeof(JUMP_CODE)]; \
    MemoryCopy(name, (unsigned char*)JUMP_CODE, sizeof(JUMP_CODE)); \
    *(UINT64*)((UINT64)name + 6) = (UINT64)targetAddress

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
// extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);

// Public Globals
// None

// Public Functions
EFI_STATUS InstallPatch(patchinfo_t *info, void *originalFunction, void *targetFunction);
EFI_STATUS UninstallPatch(patchinfo_t *info);

// Private Globals
static const unsigned char JUMP_CODE[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Private Functions
// None

// Implementation

EFI_STATUS InstallPatch(patchinfo_t *info, void *originalFunction, void *targetFunction) {
  if (!info || !originalFunction || !targetFunction)
    return EFI_INVALID_PARAMETER;

  unsigned int size = 0;

  //
  // Calculate how many bytes we need to replace with the jump instruction.
  // Keep adding instruction sizes until we have at least 14 bytes (size of JUMP_CODE).
  // Maximum x86-64 instruction is 15 bytes, so worst case is 15 + 14 = 29 bytes.
  //
  while (size < sizeof(JUMP_CODE))
    size += GetInstructionSize((unsigned char*)originalFunction + size)
  ;

  //
  // Store metadata for later restoration
  //
  info->originalFunction = originalFunction;
  info->targetFunction = targetFunction;
  info->size = size;

  //
  // Backup the original bytes we're about to overwrite
  //
  MemoryCopy(info->buffer, originalFunction, size);

  //
  // Step 1: Create jump from trampoline back to original function (after our patch).
  // This allows the original code to continue executing normally after the trampoline.
  //
  CREATE_JUMP(originalJump, (unsigned char*)originalFunction + size);
  MemoryCopy(info->buffer + size, originalJump, sizeof(JUMP_CODE));

  //
  // Disable interrupts and write protection for kernel-mode code modification
  //
  DisableInterrupts();
  DisableMemoryProtection();

  //
  // Step 2: Create jump from original function to our hook function.
  // This replaces the original code, so calls to original now call our hook.
  //
  CREATE_JUMP(hookJump, targetFunction);
  MemoryCopy(originalFunction, hookJump, sizeof(JUMP_CODE));

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

EFI_STATUS UninstallPatch(patchinfo_t *info) {
  if (!info || !info->originalFunction)
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
  MemoryCopy(info->originalFunction, info->buffer, info->size);

  //
  // Clear metadata
  //
  info->originalFunction = NULL;
  info->targetFunction = NULL;
  info->size = 0;

  //
  // Restore CPU protection mechanisms
  //
  EnableMemoryProtection();
  EnableInterrupts();

  SerialPrint("Patch uninstalled\n");

  return EFI_SUCCESS;
}
