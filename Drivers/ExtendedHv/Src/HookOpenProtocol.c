#include "ExtendedHv.h"
#include <Library/PatchWinLib.h>

// Imports
// None

// Public Globals
// None

// Public Functions
EFI_STATUS InstallHook_OpenProtocol();

// Private Globals
static EFI_OPEN_PROTOCOL gOriginal;
static VOID *gWinloadBase;

// Private Functions
static EFI_STATUS EFIAPI HookedOpenProtocol(
    IN EFI_HANDLE,
    IN EFI_GUID *,
    OUT VOID **_ OPTIONAL,
    IN EFI_HANDLE,
    IN EFI_HANDLE,
    IN UINT32
);
static VOID *GetReturnAddress();



EFI_STATUS InstallHook_OpenProtocol() {
  //
  // Ensure global is set to inital value on program start
  //
  gWinloadBase = NULL;
  
  //
  // Save original function
  //
  gOriginal = gBS->OpenProtocol;
  if (!gOriginal) {
    SerialPrint("[!] Failed to save original function pointer\n");
    return EFI_UNSUPPORTED;
  }

  //
  // Install hook
  //
  gBS->OpenProtocol = HookedOpenProtocol;
  if (gBS->OpenProtocol != HookedOpenProtocol) {
    SerialPrint("[!] Failed to install hook\n");
    return EFI_UNSUPPORTED;
  }

  SerialPrint("[+] OpenProtocol Hook Installed: 0x%016lx -> 0x%016lx\n", (UINT64)gOriginal, (UINT64)HookedOpenProtocol);

  return EFI_SUCCESS;
}

static EFI_STATUS EFIAPI HookedOpenProtocol(
  IN EFI_HANDLE handle,
  IN EFI_GUID *protocol,
  OUT VOID **interface OPTIONAL,
  IN EFI_HANDLE agentHandle,
  IN EFI_HANDLE controllerHandle,
  IN UINT32 attributes
) {
  //
  // Ensure we do not double find
  //
  if (!gWinloadBase) {
    //
    // Get Return Address
    //
    VOID *returnAddress = GetReturnAddress();
    if (!returnAddress) {
      goto _end;
    }
    SerialPrintHex("[*] Checking Return Address: ", (UINT64)returnAddress);
    
    //
    // Attempt to set gWinloadBase
    // 
    gWinloadBase = FindWinloadBase_Reverse(returnAddress); // Scans backwards from address until it finds valid header matching for winload

    //
    // Unhook OpenProtocol to boot slightly faster
    // 
    if (gWinloadBase) {
      gBS->OpenProtocol = gOriginal;
    } else {
      goto _end;
    }

    //
    // TODO: Patch Something Here, for now we print success
    //
    SerialPrintHex("[+] Found winload.efi in memory: ", (UINT64)gWinloadBase);
  }

_end:
  gWinloadBase = NULL;
  // 
  // Return to original
  // 
  return gOriginal(handle, protocol, interface, agentHandle, controllerHandle, attributes);  
}

static VOID *GetReturnAddress() {
  VOID *returnAddress = NULL;
#if defined(__GNUC__)
  __asm__ volatile ("movq 8(%%rbp), %0" : "=r"(returnAddress));
#elif defined(_MSC_VER)
  returnAddress = _ReturnAddress();
#else
  // TODO: Make this a compile time error if possible
  SerialPrint("[!] You have compiled with no way to get return address, Ensure you are using a supported toolchain on compilation")
#endif
  return returnAddress;
}


