# Extending Windows Hyper Visor

## Overview

This DXE_RUNTIME_DRIVER provides functionality for direct user control at ring -1 permission level. 

The driver exists in two main stages, driver loading and driver execution.

In stage 1, loading from the shell - [Check Quickstart Here](#quickstart) - the driver hooks the `OpenProtocol` and registers for the `VirtualMemoryAddresssChange` event.

Afterwards, in stage 2, the drier gets called via the `OpenProtocol` hook, then, proceeding to capture the return address - via inline assembly or built in for reading [rsp] on call - where the caller is known to potentially be winload.efi

With the return address, we now have knowledge of where winload.efi is loaded by working from the address backwards until we find a valid image, saving the result to `gWinloadBase` and closing the hook with an added guard of not searching again even if called.

Finally, now we know the winload.efi base we proceed to patch `BlLdrLoadImage` which will, at some point, load `hvic.sys` allowing us knowledge of our final destination point.

Once `hvis.sys` has been found, we simply patch it, like winload.efi, exposing our own functionality through a vmexit handler. The vmexit handler can be called by multiple instructions but the one we will focus on is `CPUID`.

The end result, a user program can call to `CPUID` with a special value in a register which our own code patched into HVIC will handle within the ring -1 space and allow, for example, execution of arbitrary ring -1 code, reading of programs memory from ring -1 space, writing to programs memory from ring -1 space, etc... 

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

**Copy either Shell_*.efi application and the ExtendedHv.efi driver into your USB stick**
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
