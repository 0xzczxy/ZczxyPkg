// For GCC/Clang (EDK2 GCC toolchain)
#ifdef __GNUC__
#define EXECUTABLE_SECTION __attribute__((section(".text")))
#endif

// For MSVC (EDK2 MSVC toolchain)
#ifdef _MSC_VER
#pragma code_seg(".text")
#define EXECUTABLE_SECTION
#endif

// Define patchinfo_t with executable buffer
typedef struct {
    unsigned char buffer[512];  // Trampoline + original code buffer
    void *trampoline;
    void *original_function;
    void *target_function;
    UINT64 size;
} patchinfo_t;

// Place in executable section
EXECUTABLE_SECTION static patchinfo_t gBlLdrLoadImagePatchInfo = {0};

// Public Globals
BOOLEAN gBlLdrLoadImageReached = FALSE;

// Public Functions
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction);

// Private Functions
static EFI_STATUS PatchedBlLdrLoadImage(
    VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
    VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
    VOID* arg14, VOID* arg15, VOID* arg16, VOID* arg17
);

// Imports
extern VOID EFIAPI SerialPrint(IN CONST CHAR8 *format, ...);
extern VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *label, IN UINT64 value);
extern EFI_STATUS InstallPatch(patchinfo_t *info, void *originalFunction, void *targetFunction);

// Implementation
EFI_STATUS InstallPatch_BlLdrLoadImage(IN VOID *originalFunction) {
    EFI_STATUS status;
    
    SerialPrint("[+] Installing patch, buffer at 0x%p\n", &gBlLdrLoadImagePatchInfo.buffer);
    
    // Install Patch using patching utilities
    status = InstallPatch(&gBlLdrLoadImagePatchInfo, originalFunction, PatchedBlLdrLoadImage);
    
    if (EFI_ERROR(status)) {
        SerialPrint("[!] InstallPatch failed: %r\n", status);
        return status;
    }
    
    SerialPrint("[+] Patch installed successfully\n");
    
    // DEBUG: Verify the buffer was written correctly
    SerialPrint("[DEBUG] Dumping trampoline buffer:\n");
    unsigned char *buf = (unsigned char *)gBlLdrLoadImagePatchInfo.buffer;
    SerialPrint("[DEBUG] First 32 bytes of buffer:\n");
    for (int i = 0; i < 32; i++) {
        if (i % 16 == 0) SerialPrint("[DEBUG]   ");
        SerialPrint("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) SerialPrint("\n");
    }
    
    SerialPrintHex("[DEBUG] Trampoline pointer", (UINT64)gBlLdrLoadImagePatchInfo.trampoline);
    SerialPrintHex("[DEBUG] Original function", (UINT64)gBlLdrLoadImagePatchInfo.original_function);
    SerialPrintHex("[DEBUG] Size", gBlLdrLoadImagePatchInfo.size);
    
    return status;
}

static EFI_STATUS PatchedBlLdrLoadImage(
    VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, VOID* arg5, VOID* arg6, VOID* arg7,
    VOID* arg8, VOID* arg9, VOID* arg10, VOID* arg11, VOID* arg12, VOID* arg13,
    VOID* arg14, VOID* arg15, VOID* arg16, VOID* arg17
) {
    SerialPrint("[+] Hook Entered.\n");
    SerialPrintHex("[+] Buffer Address", (UINT64)&gBlLdrLoadImagePatchInfo.buffer);
    SerialPrintHex("[+] Trampoline Address", (UINT64)gBlLdrLoadImagePatchInfo.trampoline);
    SerialPrintHex("[+] Size copied", gBlLdrLoadImagePatchInfo.size);
    
    // DEBUG: Dump first few bytes before executing
    unsigned char *buf = (unsigned char *)gBlLdrLoadImagePatchInfo.trampoline;
    SerialPrint("[DEBUG] About to execute from 0x%p\n", buf);
    SerialPrint("[DEBUG] First 16 bytes: ");
    for (int i = 0; i < 16; i++) {
        SerialPrint("%02x ", buf[i]);
    }
    SerialPrint("\n");
    
    // Call the original function via trampoline
    SerialPrint("[+] Calling trampoline...\n");
    const EFI_STATUS status =
        (
            (EFI_STATUS(*)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*))
            gBlLdrLoadImagePatchInfo.trampoline
        )
        (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17);
    
    SerialPrint("[+] Trampoline Returned with status: %r\n", status);
    
    if (EFI_ERROR(status)) return status;
    
    SerialPrint("[+] Completed successfully.\n");
    gBlLdrLoadImageReached = TRUE;
    SerialPrint("[+] Wrote to BlLdrLoadImageReached.\n");
    
    return status;
}
