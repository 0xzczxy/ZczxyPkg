#include "ExtendedHv.h"

// Import
extern EFI_STATUS InstallHook_LoadImage(VOID);
extern EFI_STATUS InstallHook_StartImage(VOID);

// Public Globals
_self_state_t gSelfState;

// Publilc Functions
EFI_STATUS EFIAPI DriverEntry(IN EFI_HANDLE, IN EFI_SYSTEM_TABLE *);

// Private Globals
// None

// Private Functions
static VOID EFIAPI VirtualAddressChangeEvent(IN EFI_EVENT, IN VOID *);

// Entry Point
EFI_STATUS EFIAPI DriverEntry(IN EFI_HANDLE imageHandle, IN EFI_SYSTEM_TABLE *systemTable) {
  EFI_STATUS status = EFI_SUCCESS;
  EFI_EVENT virtualAddressChangeEvent;

  //
  // Initialize context
  //
  gSelfState.signature = _SELF_STATE_SIGNATURE;

  //
  // Initialize serial port
  //
  SerialPortInitialize();

  //
  // Print Banner
  //
  Print(
    L"\n[*] Extended Hyper-V Loading...\n"
    L"[*] You have decided to patch into Windows HyperV to extend functionality with\n"
    L"[*] code not supported by Microsoft and made with no license to its functioning\n"
    L"[*] stability and NO liability or warranty of any kind.\n"
    L"[*]\n"
    L"[*] Any harm you, or an unintended user causes to your system utilizing the\n"
    L"[*] extended user privileges made available, can be held to your account.\n"
    L"[*]\n"
    L"[*] Final Warning: NO WARRANTY OR LIABILITY\n"
    L"[*] If you are not happy with this then use 'unload DriverName.efi'\n"
    L"[*] Do not continue to proceed without technical experience.\n"
    L"[*]\n"
    L"[*] Serial output enabled on COM1 (0x3F8)\n"
  );

  //
  // Send initial message to serial
  //
  SerialPrint("\n");
  SerialPrint("================================================\n");
  SerialPrint("  Extended Hyper-V Driver v1.0\n");
  SerialPrint("  Serial Debug Output Initialized\n");
  SerialPrint("================================================\n");
  SerialPrintHex("ImageHandle", (UINT64)imageHandle);
  SerialPrintHex("SystemTable", (UINT64)systemTable);
  SerialPrintHex("BootServices", (UINT64)gBS);
  SerialPrintHex("RuntimeServices", (UINT64)gRT);

  // 
  // Install Hook on LoadImage
  //
  status = InstallHook_LoadImage();
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to install 'LoadImage' hook: %r\n", status);
    Print(L"[!] Failed to install 'LoadImage' hook: %r\n", status);
  }

  //
  // Install Hook on StartImage
  // 
  status = InstallHook_StartImage();
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to install 'StartImage' hook: %r\n", status);
    Print(L"[!] Failed to install 'StartImage' hook: %r\n", status);
  }

  //
  // Register for virtual address change event
  //
  status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_NOTIFY,
    VirtualAddressChangeEvent,
    NULL, // Optional Context to pass to Event, Currently unused
    &gEfiEventVirtualAddressChangeGuid,
    &virtualAddressChangeEvent
  );
  if (EFI_ERROR(status)) {
    SerialPrint("[!] Failed to register VirtualAddressChangeEvent: %r\n", status);
    Print(L"[!] Failed to register event for 'VirtualAddressChangeEvent': %r\n", status);
  }

  //
  // Print Next Steps after driver loaded
  //
  Print(L"[+] Driver loaded successfully\n");
  Print(L"[*] Type 'exit' to boot Windows\n\n");
  
  SerialPrint("[+] Driver initialization complete\n");
  SerialPrint("[*] Waiting for boot process to continue...\n\n");

  // 
  // Set Checkpoint
  // 
  gSelfState.checkpoint = SSTATE_CHECKPOINT_LOADED;

  //
  // Return Status
  //
  return status;
}

VOID EFIAPI VirtualAddressChangeEvent(IN EFI_EVENT event, IN VOID *context) {
  SerialPrint("\n[*] VirtualAddressChangeEvent triggered\n");
  SerialPrint("[*] Converting pointers to virtual addresses...\n");
  
  //
  // Example:
  // gRT->ConvertPointer(0, (VOID **)&gSomeGlobalPointer);
  //
  
  SerialPrint("[+] Virtual address conversion complete\n");
}

