# Extending Windows Hyper Visor

## Overview

This DXE_RUNTIME_DRIVER provides functionality for direct user control at VTL 1 permission level, the new highest permission level in Windows 11.

**WARNING: This is inrecibly dangerous and could cause, system instability, potential copywright violations (depending on juridstication), or could fall to exploitation by malicous actors.**
**WARNING: DO NOT UNDER ANY CIRCUMSTANCES INSTALL THIS WITHOUT KNOWING WHAT IT DOES AND PROPERLY UNDERSTANDING THE RISK.**
**WARNING: CONSULT PROPER LEGAL AND TECHNOLOGICAL ADVISEMENT ON THIS PACKAGE FROM VERIFIED EXPERTS, WHICH THE CREATORS ARE NOT.**
**WARNING: DO NOT ATTEMPT TO TAKE ANYTHING WE SAY AS PROFESSIONAL ADVICE.**
**WARNING: WE DO NOT GIVE ANY LIABILITY OR WARRANTY, EXPLICIT OR IMPLICIT, NOT EVEN TO THE PROPER FUNCTIONING OF THE DRIVER.**


## Quickstart

**Clone EDK2**
```
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule --init --update
```

**Make Base Tools**
```
make -C BaseTools
```

**Source edk2 script**
```
source edksetup.sh
```

**Compile Shell Package**
```
build -a X64 -t GCC5 -p ShellPkg/ShellPkg.dsc
```

**Compile Zczxy Package**
```
build -a X64 -t GCC5 -p ZczxyPkg/ZczxyPkg.dsc
```

**Copy Shell_E....efi application and the ExtendedHv.efi driver into your USB stick**
```
TODO
```

**Boot from the USB stick (Motherboard Specific)**
```
Motherboard Specific, No Help Provided, Check your motherboard UEFI settings help guide.
```

**Once in the Shell, Use the 'load' command to load the driver**
```
load Path\To\Driver\ExtendedHv.efi
```

**Finally, use the 'exit' command and let it boot into windows**
```
exit
# Note UEFI will automatically go to the next boot option which should be windows boot manager
# or (in the case of grub) allow a screen to select the windows boot manager.
```

## Implementation Details

**Control Flow**
```markdown
Main:
- InstallHook_ExitBootServices
- InstallHook_GetVariable

GetVariableHook:
- InstallPatch_BlImgAllocateImageBuffer
- InstallPatch_BlLdrLoadImage

BlImgAllocateImageBuffer:
- Extend allocation for hypervisor by our payload size.

BlLdrLoadImage:
- AddSection
- InstallPayload
- PatchCallInstruction
```

Once the payload is installed it will sit on the vmexit functionality, intercepting calls for CPUID with a specific leaf value to create a hypercall interface.

The installation of the payload is very simplistic, I am sure you have probably seen this before in multiple different programs, the most interesting part is how the payload is managed.

Our payload must be a completely isolated section inside the hypervisor, such a section has also got to have a single function and potentially multiple globals.

We have multiple options on how to integrate our payload:
- **MSVC __ImageBase**: Copy the entire driver into the section and then scan for our own exit handler function, simplistic but bulky and MSVC specific.
- **Program Data**: Compile a program sperately, turn the bytes into a C array and embed it into our program directly, copy the bytes into the section and have the function at a fixed or findable offset (i.e. by some magic value we know will be there).
- **Assembly Module**: Include an assembly module placed in a specific section or at least all together in the same section (which should be possible via assembly level control), then copy from from two symbols the program data into our section and complete similar steps to program data.

We have been forced to utilize **Program Data** as the best option, an assembly module is unrealistic with how complex the VmExitHandler code will be, and we can't rely on __ImageBase since I personally compile and test on a linux system (nixos).

Its a bit hacky, but it does work, you compile the payload separately, generate the header file by a python script, and compile the driver with the payloads byte embedded into the .data section. The function will have to exist at a specific offset which is why we need to utilize linker scripts.



