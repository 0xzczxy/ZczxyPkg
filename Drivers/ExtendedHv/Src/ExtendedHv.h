#ifndef __EXTENDEDHV_H
#define __EXTENDEDHV_H

//
// Uefi Includes
// 
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/PeImage.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>

// 
// Allignment Macros
// 
#define ALIGN_DOWN(Value, Alignment) \
  ((Value) & ~((Alignment) - 1))

#define ALIGN_UP(Value, Alignment) \
  (((Value) + (Alignment) - 1) & ~((Alignment) - 1))

// 
// Iternal State of Being
//
// #define _SELF_STATE_SIGNATURE SIGNATURE_32('E','X','H','V') // Extend Hyper Visor
// struct _self_state {
//   // Validation of State
//   UINT32 signature;

//   // Validation of Services
//   UINT64 checkpoint; // Checkpoints Reached
// };
// typedef struct _self_state _self_state_t;

// extern _self_state_t gSelfState;

//
// Status Codes
//
// #define SSTATE_CHECKPOINT_NULL     0        // Nothing Has been done
// #define SSTATE_CHECKPOINT_LOADED   (1 << 1) // DriverEntry has finished

//
// Types
// 
// typedef struct {
//   UINT64 low;
//   UINT64 high;
// } INT128_t;


#endif // __EXTENDEDHV_H
