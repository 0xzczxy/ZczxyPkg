#include "ExtendedHv.h"
#include "Compiler.h"
#include "WinDefines.h"
#include "../Payload/payload_data.h"

#define ARCH_UNKNOWN 0
#define ARCH_INTEL 1
#define ARCH_AMD 2

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern UINT64 PeAddSection(IN UINT64 imageBase, IN CONST CHAR8* sectionName, IN UINT32 virtualSize, IN UINT32 characteristics);
extern UINT64 FindPatternImage(IN VOID* imageBase, IN CONST CHAR8* pattern);
extern UINT64 FindPattern(IN VOID* base, IN UINTN size, IN CONST CHAR8* pattern);

// Public Functions
EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize);
UINT64 PatchSizeVmExitHandler(VOID);

// Private Functions
static int DetectArchitecture(IN VOID* imageBase);
static EFI_STATUS PatchIntel(IN UINT64 imageBase, IN UINT64 section);
static EFI_STATUS PatchAmd(IN UINT64 imageBase, IN UINT64 section);


// Implementation
UINT64 PatchSizeVmExitHandler(VOID) {
  return PAYLOAD_SIZE;
}

EFI_STATUS InstallPatch_VmExitHandler(UINT64 imageBase, UINT64 imageSize) {
  UINT64 section;
  int arch;
  EFI_STATUS status;

  //
  // Detect architecture
  // 
  arch = DetectArchitecture((VOID*)imageBase);
  if (arch == ARCH_INTEL) {
    SerialPrint("[+] Intel Architecture Detected.\n");
  } else if (arch == ARCH_AMD) {
    SerialPrint("[+] AMD Architecture Detected.\n");
  } else {
    SerialPrint("[!] Failed to detect Architecture.\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Add payload section
  // 
  section = PeAddSection(imageBase, ".zczxyhc", PatchSizeVmExitHandler(), SECTION_RWX);
  if (!section) {
    SerialPrint("[!] Failed to add payload section\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Apply architecture specific patch
  //
  if (arch == ARCH_INTEL) {
    status = PatchIntel(imageBase, section);
    if (EFI_ERROR(status)) SerialPrint("[!] Failed to patch hvix64.exe\n");
  } else {
    status = PatchAmd(imageBase, section);
    if (EFI_ERROR(status)) SerialPrint("[!] Failed to patch hvax64.exe\n");
  }
  SerialPrint("[+] Patch hv*x64.exe successfully.\n");


  //
  // Copy payload (g_payload_data is patched from PatchIntel/PatchAmd)
  // 
  DisableMemoryProtection();
  CopyMem((VOID*)section, g_payload_data, PAYLOAD_SIZE);
  FlushInstructionCache((VOID*)section, PAYLOAD_SIZE);
  EnableMemoryProtection();

  SerialPrint("[+] Patching succeeded.\n");

  return EFI_SUCCESS;
}

static int DetectArchitecture(IN VOID* imageBase) {
  //
  // Intel: Look for VMRESUME
  // 
  if (FindPatternImage(imageBase, "0F 01 C3") != 0) {
    return ARCH_INTEL;
  }

  //
  // AMD: Look for VMRUN
  // 
  if (FindPatternImage(imageBase, "0F 01 D8") != 0) {
    return ARCH_AMD;
  }

  return ARCH_UNKNOWN;
}

static EFI_STATUS PatchIntel(IN UINT64 imageBase, IN UINT64 section) {
  UINT64 vmwriteAddr;
  UINT64 leaAddr;
  
  //
  // Find VMWRITE instruction that sets HOST_RIP (0x6c16)
  // Pattern: mov eax, 0x6c16 ; vmwrite rax, rcx
  //
  vmwriteAddr = FindPatternImage((VOID*)imageBase, "B8 16 6C 00 00 0F 79 C1");
  if (!vmwriteAddr) {
    SerialPrint("[!] HOST_RIP VMWRITE not found.\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Found VMWRITE at 0x%llx\n", vmwriteAddr);
  
  //
  // Back up to find the LEA instruction that loads the handler address
  // Pattern: lea rcx, [rip+offset]  (48 8D 0D)
  //
  leaAddr = vmwriteAddr;
  for (int i = 0; i < 32; i++) {  // Search backwards max 32 bytes
    leaAddr--;
    if (*(UINT8*)leaAddr == 0x48 && 
        *(UINT8*)(leaAddr + 1) == 0x8D && 
        *(UINT8*)(leaAddr + 2) == 0x0D) {
      break;
    }
  }
  
  if (leaAddr == vmwriteAddr - 32) {
    SerialPrint("[!] LEA instruction not found.\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Found LEA at 0x%llx\n", leaAddr);
  
  //
  // Calculate addresses (same logic as AMD)
  //
  const UINT64 leaBase = leaAddr + 7;  // LEA is 7 bytes: 48 8D 0D XX XX XX XX
  const INT32 originalOffset = *(INT32*)(leaAddr + 3);
  const UINT64 originalHandler = leaBase + originalOffset;
  const UINT64 hookedHandler = section + PAYLOAD_FUNCTION_OFFSET;
  const INT32 newOffset = (INT32)(hookedHandler - leaBase);
  const INT64 offsetToOriginal = (INT64)(originalHandler - hookedHandler);
  
  SerialPrint("[+] Original handler: 0x%llx\n", originalHandler);
  SerialPrint("[+] Hooked handler: 0x%llx\n", hookedHandler);
  SerialPrint("[+] Offset to original: 0x%llx\n", offsetToOriginal);
  
  //
  // Patch payload globals
  //
  INT64 *globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)g_payload_data);
  *globalOffset = offsetToOriginal;
  
  INT32 *archOffset = PAYLOAD_ARCH_PTR((VOID*)g_payload_data);
  *archOffset = ARCH_INTEL;
  
  //
  // Patch the LEA instruction's offset
  //
  DisableMemoryProtection();
  *(INT32*)(leaAddr + 3) = newOffset;
  FlushInstructionCache((VOID*)leaAddr, 7);
  EnableMemoryProtection();
  
  SerialPrint("[+] INTEL Patched LEA instruction successfully.\n");
  
  return EFI_SUCCESS;
}

static EFI_STATUS PatchAmd(IN UINT64 imageBase, IN UINT64 section) {
  UINT64 callAddr;

  //
  // Find pattern for the relative call that has not changed since the release of voyager
  // https://github.com/backengineering/Voyager/blob/master/Voyager/Voyager/Hv.h
  //
  callAddr = FindPatternImage((VOID*)imageBase, "E8 ? ? ? ? 48 89 04 24 E9");
  if (!callAddr) {
    SerialPrint("[!] Pattern not found.\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Found pattern at 0x%p\n", callAddr);

  //
  // Get the information for patching the call instruction
  // (this is very obtuse)
  // 
  const UINT64 callBase = callAddr + 5;
  const INT32 originalOffset = *(INT32*)(callAddr + 1);
  const UINT64 originalFunction = callBase + originalOffset;
  const UINT64 hookedFunction = section + PAYLOAD_FUNCTION_OFFSET;
  const INT32 newOffset = (INT32)(hookedFunction - callBase);
  const INT64 offsetToOriginal = (INT64)(originalFunction - hookedFunction);

  //
  // Patch payload globals
  //
  INT64 *globalOffset = PAYLOAD_GLOBAL_PTR((VOID*)g_payload_data);
  *globalOffset = offsetToOriginal;
  
  INT32 *archOffset = PAYLOAD_ARCH_PTR((VOID*)g_payload_data);
  *archOffset = ARCH_AMD;

  //
  // Patch the call within hvax64.exe
  // 
  DisableMemoryProtection();
  *(INT32*)(callAddr + 1) = newOffset;
  FlushInstructionCache((VOID*)callAddr, 5);
  EnableMemoryProtection();

  SerialPrint("[+] AMD Patched CALL instruction successfully.\n");

  return EFI_SUCCESS;
}


