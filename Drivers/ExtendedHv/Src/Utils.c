#include "ExtendedHv.h"

// Imports
// None

// Public Globals
// None

// Public Functions
CHAR16 *StriStr(IN CONST CHAR16 *string, IN CONST CHAR16 *searchString);

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
