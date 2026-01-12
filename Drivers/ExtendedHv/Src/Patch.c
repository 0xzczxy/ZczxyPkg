#include "Compiler.h"
#include "ExtendedHv.h"

#define CREATE_JUMP(name, targetAddress) \
    unsigned char name[sizeof(JUMP_CODE)]; \
    CopyMem(name, (unsigned char*)JUMP_CODE, sizeof(JUMP_CODE)); \
    *(UINT64*)((UINT64)name + 6) = (UINT64)targetAddress

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);

// Public Functions
EFI_STATUS InstallPatch(OUT patchinfo_t *info, IN VOID *originalFunction, IN VOID *targetFunction);
EFI_STATUS UninstallPatch(IN OUT patchinfo_t *info);

// Private Globals
static const unsigned char JUMP_CODE[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Implementation

EFI_STATUS InstallPatch(OUT patchinfo_t *info, IN VOID *originalFunction, IN VOID *targetFunction) {
  if (!info || !originalFunction || !targetFunction)
    return EFI_INVALID_PARAMETER;

  //
  // Disable interrupts and write protection for kernel-mode code modification
  //
  DisableInterrupts();
  DisableMemoryProtection();

  //
  // Store metadata for later restoration
  //
  info->original_function = originalFunction;
  info->target_function = targetFunction;

  //
  // Backup the original bytes we're about to overwrite (14 bytes for the jump)
  //
  CopyMem(info->buffer, originalFunction, sizeof(JUMP_CODE));

  //
  // Create and install jump from original function to our hook function
  // This replaces the original code, so calls to original now call our hook
  //
  CREATE_JUMP(hookJump, targetFunction);
  CopyMem(originalFunction, hookJump, sizeof(JUMP_CODE));

  //
  // Restore CPU protection mechanisms
  //
  EnableMemoryProtection();
  EnableInterrupts();

  SerialPrint(
    "Patch installed: 0x%p -> 0x%p\n", 
    originalFunction, targetFunction
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
  CopyMem(info->original_function, info->buffer, sizeof(JUMP_CODE));

  //
  // Restore CPU protection mechanisms
  //
  EnableMemoryProtection();
  EnableInterrupts();

  SerialPrint("Patch uninstalled\n");

  return EFI_SUCCESS;
}
