#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString);
UINT64 DisableWriteProtection(VOID);
VOID RestoreWriteProtection(UINT64 originalCr0);

// Private Globals
// None

// Private Functions
// None



CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString) {
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

#define CR0_WP_BIT (1ULL << 16)  // Write Protect bit

UINT64 DisableWriteProtection(VOID) {
  UINT64 cr0;
  
  //
  // Read CR0
  //
  cr0 = AsmReadCr0();
  
  SerialPrintHex("[*] Original CR0", cr0);
  
  //
  // Clear WP bit
  //
  AsmWriteCr0(cr0 & ~CR0_WP_BIT);
  
  SerialPrintHex("[*] Modified CR0", AsmReadCr0());
  SerialPrint("[+] Write protection disabled\n");
  
  return cr0;  // Return original value for restoration
}

VOID RestoreWriteProtection(UINT64 originalCr0) {
  SerialPrint("[*] Restoring write protection...\n");
  AsmWriteCr0(originalCr0);
  SerialPrintHex("[*] Restored CR0", AsmReadCr0());
  SerialPrint("[+] Write protection restored\n");
}
