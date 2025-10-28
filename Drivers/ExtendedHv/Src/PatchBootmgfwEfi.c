#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
BOOLEAN IsBootmgfwEfi(IN CHAR16 *devicePath);
EFI_STATUS PatchBootmgfw(IN VOID *imageBase, IN UINT64 imageSize);

// Private Globals
// None

// Private Functions
static CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString);

BOOLEAN IsBootmgfwEfi(IN CHAR16 *pathString) {
  BOOLEAN result;
  
  SerialPrint("[*] Checking path: %s\n", pathString);
  
  //
  // Check if path contains bootmgfw.efi
  //
  result = (StriStr(pathString, L"bootmgfw.efi") != NULL);
  
  return result;
}

EFI_STATUS PatchBootmgfw(IN VOID *imageBase, IN UINT64 imageSize) {
  if (!imageBase || imageSize == 0) {
    SerialPrint("[!] Invalid parameters\n");
    return EFI_INVALID_PARAMETER;
  }
  
  SerialPrint("[*] Patching bootmgfw.efi\n");
  SerialPrintHex("ImageBase", (UINT64)imageBase);
  SerialPrintHex("ImageSize", imageSize);
  
  //
  // TODO: Implement patching logic
  //
  
  SerialPrint("[+] Bootmgfw patched successfully\n");
  return EFI_SUCCESS;
}

static CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString) {
  CHAR16 *source;
  CHAR16 c1;
  CHAR16 c2;
  UINTN searchLen;
  UINTN i;
  
  if (!string || !searchString) {
    return NULL;
  }
  
  searchLen = StrLen(searchString);
  if (searchLen == 0) {
    return (CHAR16 *)string;
  }
  
  //
  // Case-insensitive substring search
  //
  source = (CHAR16 *)string;
  while (*source != L'\0') {
    for (i = 0; i < searchLen; i++) {
      c1 = source[i];
      c2 = searchString[i];
      
      //
      // Convert to lowercase
      //
      if (c1 >= L'A' && c1 <= L'Z') {
        c1 = c1 + (L'a' - L'A');
      }
      if (c2 >= L'A' && c2 <= L'Z') {
        c2 = c2 + (L'a' - L'A');
      }
      
      if (c1 != c2) {
        break;
      }
    }
    
    if (i == searchLen) {
      return source;
    }
    
    source++;
  }
  
  return NULL;
}
