//
// Complete BlLdrLoadImage Hook for Secure Kernel Patching
// Based on reverse engineering findings
//

#include "ExtendedHv.h"

// Imports
extern CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString);
extern VOID *FindPatternPeExecutableSections(
  IN VOID *imageBase, IN UINT64 imageSize,
  IN VOID *pattern, IN VOID *mask,
  IN UINT64 pattern_size
);
extern EFI_STATUS InstallPatch(IN VOID *targetFunction, IN VOID *patchFunction, OUT UINT8 *originalBytes);
extern UINT64 DisableWriteProtection(VOID);
extern VOID RestoreWriteProtection(UINT64 originalCr0);

//
// Boot loader data table entry structure
//
typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;           // +0x00
    LIST_ENTRY InMemoryOrderLinks;         // +0x10
    LIST_ENTRY InInitializationOrderLinks; // +0x20
    VOID *DllBase;                         // +0x30 ← Image base
    VOID *EntryPoint;                      // +0x38
    UINTN SizeOfImage;                     // +0x40
} LDR_DATA_TABLE_ENTRY;

//
// BlLdrLoadImage function signature
//
typedef UINT64 (EFIAPI *BlLdrLoadImage_t)(
    VOID *arg_01, VOID *arg_02, VOID *arg_03, VOID *arg_04,
    VOID *arg_05, VOID *arg_06, VOID *arg_07, VOID *arg_08,
    VOID *arg_09, VOID *arg_10, VOID *arg_11, VOID *arg_12,
    VOID *arg_13, VOID *arg_14, VOID *arg_15, VOID *arg_16
);

// Pattern for BlLdrLoadImage at offset 0x1801981fc
static UINT8 gBlLdrLoadImagePattern[] = {
	0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x08, 0x48, 0x89, 0x70, 0x10, 0x48, 0x89, 0x78, 0x18, 0x55,
	0x48, 0x8d, 0x68, 0xf1, 0x48, 0x81, 0xec, 0xc0, 0x00, 0x00, 0x00, 0x8b, 0xf1, 0xc6, 0x45, 0xd7,
	0x00, 0x49, 0x8b, 0xc1, 0x48, 0x8d, 0x4d, 0xd7, 0x48, 0x89, 0x4c, 0x24, 0x28, 0x4c, 0x8d, 0x4d,
	0xef, 0x48, 0x8d, 0x4d, 0xdf, 0x48, 0x8b, 0xfa, 0x48, 0x89, 0x4c, 0x24, 0x20, 0x0f, 0x57, 0xc0,
	0x0f, 0x57, 0xc9, 0x49, 0x8b, 0xc8, 0x48, 0x8b, 0xd0, 0x0f, 0x11, 0x45, 0xdf, 0x0f, 0x11, 0x4d,
	0xef, 0xe8, 0x2a, 0xf6, 0xff, 0xff, 0x8b, 0xd8, 0x85, 0xc0, 0x0f, 0x88, 0x90, 0x00, 0x00, 0x00,
	0x48, 0x8b, 0x85, 0x97, 0x00, 0x00, 0x00, 0x4c, 0x8d, 0x4d, 0xdf, 0x48, 0x89, 0x84, 0x24, 0x88,
	0x00, 0x00, 0x00, 0x4c, 0x8d, 0x45, 0xef, 0x48, 0x8b, 0x85, 0x8f, 0x00, 0x00, 0x00, 0x48, 0x8b,
	0xd7, 0x48, 0x89, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00, 0x8b, 0xce, 0x8b, 0x85, 0x87, 0x00, 0x00,
	0x00, 0x89, 0x44, 0x24, 0x78, 0x8b, 0x45, 0x7f, 0x89, 0x44, 0x24, 0x70, 0x8b, 0x45, 0x77, 0x89,
	0x44, 0x24, 0x68, 0x8b, 0x45, 0x6f, 0x89, 0x44, 0x24, 0x60, 0x8b, 0x45, 0x67, 0x89, 0x44, 0x24,
	0x58, 0x48, 0x8b, 0x45, 0x5f, 0x48, 0x89, 0x44, 0x24, 0x50, 0x48, 0x8b, 0x45, 0x57, 0x48, 0x89,
	0x44, 0x24, 0x48, 0x48, 0x83, 0x64, 0x24, 0x40, 0x00, 0x48, 0x8b, 0x45, 0x4f, 0x48, 0x89, 0x44,
	0x24, 0x38, 0x8b, 0x45, 0x47, 0x89, 0x44, 0x24, 0x30, 0x8b, 0x45, 0x3f, 0x89, 0x44, 0x24, 0x28,
	0x48, 0x8b, 0x45, 0x37, 0x48, 0x89, 0x44, 0x24, 0x20, 0xe8, 0x96, 0xe8, 0xff, 0xff, 0x8b, 0xd8,
	0x80, 0x7d, 0xd7, 0x00, 0x74, 0x09, 0x48, 0x8d, 0x4d, 0xdf, 0xe8, 0x49, 0x3c, 0xff, 0xff, 0x4c,
	0x8d, 0x9c, 0x24, 0xc0, 0x00, 0x00, 0x00, 0x8b, 0xc3, 0x49, 0x8b, 0x5b, 0x10, 0x49, 0x8b, 0x73,
	0x18, 0x49, 0x8b, 0x7b, 0x20, 0x49, 0x8b, 0xe3, 0x5d, 0xc3
};

static UINT8 gBlLdrLoadImageOriginalBytes[16] = {0};
static VOID *gBlLdrLoadImageOriginal = NULL;
static BOOLEAN gSecureKernelPatched = FALSE;

// Forward declarations
static UINT64 EFIAPI HookedBlLdrLoadImage(
    VOID *arg_01, VOID *arg_02, VOID *arg_03, VOID *arg_04,
    VOID *arg_05, VOID *arg_06, VOID *arg_07, VOID *arg_08,
    VOID *arg_09, VOID *arg_10, VOID *arg_11, VOID *arg_12,
    VOID *arg_13, VOID *arg_14, VOID *arg_15, VOID *arg_16
);
static EFI_STATUS PatchSecureKernel(IN VOID *imageBase, IN UINT64 imageSize);

//
// Main patching function
//
EFI_STATUS PatchWinload(IN VOID *imageBase, IN UINT64 imageSize) {
    EFI_STATUS status;
    VOID *blLdrLoadImage;
    UINT64 originalCr0;
    
    if (!imageBase || imageSize == 0) {
        SerialPrint("[!] Invalid parameters\n");
        return EFI_INVALID_PARAMETER;
    }
    
    SerialPrint(
        "\n"
        "========================================\n"
        "  Patching Winload.efi\n"
        "========================================\n"
    );
    SerialPrintHex("ImageBase", (UINT64)imageBase);
    SerialPrintHex("ImageSize", imageSize);
    
    //
    // Find BlLdrLoadImage
    // Known offset: 0x1801981fc (adjust based on your winload version)
    //
    SerialPrint("[*] Searching for BlLdrLoadImage...\n");
    
    // Option 1: Use known offset if available
    // blLdrLoadImage = (VOID *)((UINT8 *)imageBase + 0x1981fc);
    
    // Option 2: Pattern search
    blLdrLoadImage = FindPatternPeExecutableSections(
        imageBase, imageSize,
        gBlLdrLoadImagePattern, NULL,
        sizeof(gBlLdrLoadImagePattern)
    );
    
    if (!blLdrLoadImage) {
        SerialPrint("[!] Failed to find BlLdrLoadImage\n");
        SerialPrint("[!] Winload patching failed\n");
        SerialPrint("========================================\n\n");
        return EFI_NOT_FOUND;
    }
    
    SerialPrintHex("BlLdrLoadImage", (UINT64)blLdrLoadImage);
    gBlLdrLoadImageOriginal = blLdrLoadImage;
    
    //
    // Install hook
    //
    SerialPrint("[*] Installing hook on BlLdrLoadImage...\n");
    originalCr0 = DisableWriteProtection();
    
    status = InstallPatch(
        blLdrLoadImage,
        (VOID *)HookedBlLdrLoadImage,
        gBlLdrLoadImageOriginalBytes
    );
    
    RestoreWriteProtection(originalCr0);
    
    if (EFI_ERROR(status)) {
        SerialPrint("[!] Failed to install patch: %r\n", status);
        SerialPrint("[!] Winload patching failed\n");
        SerialPrint("========================================\n\n");
        return status;
    }
    
    SerialPrint("[+] BlLdrLoadImage hooked successfully\n");
    SerialPrint("[*] Waiting for securekernel.exe load...\n");
    SerialPrint("========================================\n\n");
    
    return EFI_SUCCESS;
}

//
// BlLdrLoadImage hook
//
static UINT64 EFIAPI HookedBlLdrLoadImage(
    VOID *arg_01, VOID *arg_02, VOID *arg_03, VOID *arg_04,
    VOID *arg_05, VOID *arg_06, VOID *arg_07, VOID *arg_08,
    VOID *arg_09, VOID *arg_10, VOID *arg_11, VOID *arg_12,
    VOID *arg_13, VOID *arg_14, VOID *arg_15, VOID *arg_16
) {
    UINT64 status;
    BlLdrLoadImage_t originalFunc;
    UINT64 originalCr0;
    CHAR16 *modulePath = NULL;
    BOOLEAN isSecureKernel = FALSE;
    
    //
    // Restore original function
    //
    originalCr0 = DisableWriteProtection();
    CopyMem(gBlLdrLoadImageOriginal, gBlLdrLoadImageOriginalBytes, 16);
    RestoreWriteProtection(originalCr0);
    
    //
    // Call original
    //
    originalFunc = (BlLdrLoadImage_t)gBlLdrLoadImageOriginal;
    status = originalFunc(
        arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08,
        arg_09, arg_10, arg_11, arg_12, arg_13, arg_14, arg_15, arg_16
    );
    
    //
    // Re-install hook
    //
    originalCr0 = DisableWriteProtection();
    UINT8 *target = (UINT8 *)gBlLdrLoadImageOriginal;
    UINT64 patchAddr = (UINT64)HookedBlLdrLoadImage;
    target[0] = 0xFF;
    target[1] = 0x25;
    target[2] = 0x00;
    target[3] = 0x00;
    target[4] = 0x00;
    target[5] = 0x00;
    CopyMem(&target[6], &patchAddr, sizeof(UINT64));
    RestoreWriteProtection(originalCr0);


    //
    // Check ALL possible path locations
    //
    
    // Try arg_02 (direct path - rare)
    if (arg_02 && (UINT64)arg_02 > 0x1000) {
        modulePath = (CHAR16 *)arg_02;
    }
    
    // Try arg_03 (more common based on disassembly)
    if (!modulePath && arg_03 && (UINT64)arg_03 > 0x1000) {
        // arg_03 might be a structure containing the path
        // Try as direct CHAR16*
        CHAR16 *test = (CHAR16 *)arg_03;
        if (test[0] > 0x20 && test[0] < 0x7F) {
            modulePath = test;
        }
    }
    
    // Try arg_04 similarly
    if (!modulePath && arg_04 && (UINT64)arg_04 > 0x1000) {
        CHAR16 *test = (CHAR16 *)arg_04;
        if (test[0] > 0x20 && test[0] < 0x7F) {
            modulePath = test;
        }
    }
    
    //
    // Extract filename if we found a path
    //
    if (modulePath) {
        CHAR16 *filename = modulePath;
        for (CHAR16 *p = modulePath; *p != 0 && (p - modulePath) < 512; p++) {
            if (*p == L'\\' || *p == L'/') {
                filename = p + 1;
            }
        }
        
        isSecureKernel = (StriStr(filename, L"securekernel") != NULL);
        
        // Only log interesting files to reduce spam
        if (isSecureKernel || StriStr(filename, L".exe") || StriStr(filename, L".sys")) {
            SerialPrint("[*] BlLdrLoadImage: %s%s\n", 
                filename,
                isSecureKernel ? " ← TARGET FOUND!" : "");
        }
    }
    
    //
    // Check arg_08 for LDR entry AFTER successful call
    //
    if (isSecureKernel && status == 0 && arg_08) {
        if (!gSecureKernelPatched) {
            gSecureKernelPatched = TRUE;
            
            LDR_DATA_TABLE_ENTRY *ldrEntry = *(LDR_DATA_TABLE_ENTRY **)arg_08;
            
            if (ldrEntry && ldrEntry->DllBase) {
                SerialPrint("\n************************************************\n");
                SerialPrint("  SECURE KERNEL DETECTED!\n");
                SerialPrint("************************************************\n");
                SerialPrintHex("  DllBase", (UINT64)ldrEntry->DllBase);
                SerialPrintHex("  EntryPoint", (UINT64)ldrEntry->EntryPoint);
                SerialPrintHex("  SizeOfImage", ldrEntry->SizeOfImage);
                
                // Patch it NOW
                EFI_STATUS patchStatus = PatchSecureKernel(
                    ldrEntry->DllBase, 
                    ldrEntry->SizeOfImage
                );
                
                if (!EFI_ERROR(patchStatus)) {
                    SerialPrint("[+] *** SECURE KERNEL PATCHED! ***\n");
                } else {
                    SerialPrint("[!] Patch failed: %r\n", patchStatus);
                }
                SerialPrint("************************************************\n\n");
            }
        }
    }
    
    return status;
}

//
// Secure kernel patching implementation
//
static EFI_STATUS PatchSecureKernel(IN VOID *imageBase, IN UINT64 imageSize) {
    SerialPrint("[*] PatchSecureKernel called\n");
    SerialPrintHex("  Base", (UINT64)imageBase);
    SerialPrintHex("  Size", imageSize);
    
    //
    // TODO: Implement your secure kernel patches here
    //
    // Example targets in securekernel.exe:
    // 1. VTL (Virtual Trust Level) transition handlers
    // 2. Secure service dispatch tables
    // 3. Credential Guard policy enforcement
    // 4. Device Guard code integrity checks
    // 5. VSM (Virtual Secure Mode) initialization
    //
    
    SerialPrint("[*] TODO: Add secure kernel patches\n");
    SerialPrint("[*] Current functionality: Detection only\n");
    
    return EFI_SUCCESS;
}
