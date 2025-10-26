# UEFI C Style Guide

## File Structure

Every `.c` file follows this order:

```c
#include "Header.h"

// Imports
extern RETURN_TYPE ExternalFunction(VOID);

// Public Globals
TYPE gPublicVariable;

// Public Functions
EFI_STATUS PublicFunction(VOID);

// Private Globals
static TYPE gPrivateVariable;

// Private Functions
static RETURN_TYPE PrivateFunction(VOID);

// Implementation
EFI_STATUS PublicFunction(VOID) {
  // ...
}

static RETURN_TYPE PrivateFunction(VOID) {
  // ...
}
```

Use `// None` for empty sections.

---

## Naming

### Functions
```c
VOID EFIAPI WhatItDoes(); // Describe what the function does
VOID EFIAPI WhatItDoes_ToWhat(); // If what the function does is abstract specify after an underscore

// for example
VOID EFIAPI SerialPrint(IN CONST CHAR8 *string, ...); // Prints to screen
VOID EFIAPI SerialPrintHex(IN CONST CHAR8 *string, IN UINT64 value); // Prints a hexadecimal value

// more complex
InstallHook(VOID); // Not Explicit
InstallHookExitBootServics(VOID); // Hard to Read
InstallHook_ExitBootServices(VOID); // Clear seperation of what it does and to what

// Private Functions must be specified with static 
static VOID HelperFunction(VOID);       // Private
```

### Variables
```c
gGlobalVariable              // Global (g prefix + PascalCase)
gOriginal                    // Make Globals Minimal But Explicit, For example this refers to the original function defined privately in HookExitBootServices.c, there is of course no other hook specified here
localVariable                // Local, MUST utilize camelCase, this is different from standard PascalCase used by EDK2
functionParameter            // Parameters, MUST camelCase, this is different from standard PascalCase used by EDK2
```

### Types
```c
struct type_name {
  
};
typedef struct type_name type_name_t; // suffix_t for types

// Struct definition (ALWAYS declare corresponding struct)
// NEVER declare 'typedef struct ... {} ...;', do not inline create types but make their creation explicit and their aliases explicit

// For types used internally in the module but still included in the module header file for use across all files utilize an _prefix.
// i.e.

struct _self_state {
  
};
typedef struct _self_state _self_state_t;

struct library_type {
  
};
typedef struct library_type library_type_t;
```

### Constants & Macros
```c
// Follows types with internal definitions defined in modules header file should have an _prefix
// ALL defnitions follow SCREAMING_SNAKE_CASE

#define _INTERNAL
#define PUBLIC

// Exmaple
#define _SIGNATURE // for self validation
#define ALIGN_UP(Value, Alignment) \
  (((Value) + (Alignment) - 1) & ~((Alignment) - 1))
```

---

## Headers

### Include Guards
```c
#ifndef __MODULENAME_H
#define __MODULENAME_H

// Content

#endif // __MODULENAME_H
```

---

## Formatting

### Indentation & Braces
```c
// 2 spaces, opening brace on same line
if (condition) {
  Code();
}

while (condition) {
  Code();
}

for (i = 0; i < count; i++) {
  Code();
}
```

### Functions
```c
EFI_STATUS EFIAPI FunctionName(
  IN EFI_HANDLE handle,
  OUT VOID **result OPTIONAL
) {
  // Implementation
}

static VOID HelperFunction(VOID) {
  // Implementation
}

// if the amount of parameters are more then 2-3 then place on multiple
// lines (depending on how long it makes the line)
```

### Spacing
```c
if (condition) {              // Space after keyword
Function()                    // No space before (
a + b                        // Space around operators
*pointer                     // No space for unary
```

---

## Comments

```c
//
// Section description
//
Code();

// Inline explanation
Code();

// TODO: Future work
```

No `/* */` style comments.

---

## Diagnostic Output

**CRITICAL: Only use `SerialPrint()` for diagnostic output in drivers and libraries. Console output (Print, AsciiPrint) is unsafe after boot services exit and during early boot phases. Only application entry points should use Print().**

```c
// Success
SerialPrint("[+] Operation successful\n");

// Info
SerialPrint("[*] Processing...\n");

// Error
SerialPrint("[!] Failed: %r\n", status);

// Hex values
SerialPrintHex("Variable", (UINT64)value);

// Visual separator
SerialPrint(
  "\n"
  "========================================\n"
  "  Section Title  \n"
  "========================================\n"
  "\n"
);
```

**Application entry points only:**
```c
EFI_STATUS EFIAPI UefiMain(...) {
  Print(L"[*] Application starting\n");  // OK here
  
  DoWork();  // Uses SerialPrint internally
  
  Print(L"[+] Complete\n");  // OK here
  return EFI_SUCCESS;
}
```

---

## Error Handling

```c
// Standard pattern
status = Function();
if (EFI_ERROR(status)) {
  SerialPrint("[!] Function failed: %r\n", status);
  return status;
}

// Guard early
if (!pointer) {
  SerialPrint("[!] Null pointer\n");
  return EFI_INVALID_PARAMETER;
}

// Single exit point
if (error) {
  goto _cleanup;
}

Code();

_cleanup:
  Cleanup();
  return status;
```

---

## Common Patterns

### Saving Originals
```c
gOriginal = gBS->Function;
if (!gOriginal) {
  return EFI_UNSUPPORTED;
}

gBS->Function = HookedFunction;
if (gBS->Function != HookedFunction) {
  return EFI_UNSUPPORTED;
}
```

### Hook Implementation
```c
static EFI_STATUS EFIAPI HookedFunction(
  IN PARAM1 param1,
  IN PARAM2 param2
) {
  SerialPrint("\n[*] Function called\n");
  SerialPrintHex("Param1", (UINT64)param1);
  
  // Custom logic
  
  return gOriginal(param1, param2);
}
```

### Platform-Specific Code
```c
#if defined(__GNUC__)
  __asm__ volatile ("movq 8(%%rbp), %0" : "=r"(result));
#elif defined(_MSC_VER)
  result = _ReturnAddress();
#else
  #error "Unsupported compiler"
#endif
```

---

## Types & Casts

```c
UINT64 value = (UINT64)pointer;
EFI_STATUS status = EFI_SUCCESS;
VOID *address = NULL;
BOOLEAN condition = TRUE;
CHAR16 *wideString = L"Text";
```

---

## Quick Reference

```c
// File-private
static TYPE gVariable;
static RETURN_TYPE Function(VOID);

// Public
TYPE gVariable;
RETURN_TYPE Function(VOID);

// Parameter qualifiers
IN TYPE param              // Input
OUT TYPE *param           // Output
IN TYPE param OPTIONAL    // Optional

// Output
SerialPrint("[+] Success\n");
SerialPrint("[*] Info\n");
SerialPrint("[!] Error\n");

// Only in application entry points
Print(L"User message\n");
```
