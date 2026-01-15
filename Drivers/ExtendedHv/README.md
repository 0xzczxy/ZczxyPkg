# Extending Windows Hyper Visor

**WARNING: THIS IS INCREDIBLY DANGEROUS AND COULD CAUSE: SYSTEM INSTABILITY, POTENTIAL COPYRIGHT VIOLATIONS, OR FALL TO EXPLOITATION BY MALICOUS ACTORS.**
**WARNING: DO NOT UNDER ANY CIRCUMSTANCES INSTALL THIS WITHOUT KNOWING WHAT IT DOES AND PROPERLY UNDERSTANDING THE RISK.**
**WARNING: CONSULT PROPER LEGAL AND TECHNOLOGICAL ADVISEMENT ON THIS PACKAGE FROM VERIFIED EXPERTS, WHICH THE CREATORS ARE NOT.**
**WARNING: DO NOT ATTEMPT TO TAKE ANYTHING WE SAY AS PROFESSIONAL ADVICE.**
**WARNING: WE DO NOT GIVE ANY LIABILITY OR WARRANTY, EXPLICIT OR IMPLICIT, NOT EVEN TO THE PROPER FUNCTIONING OF THE DRIVER.**

## LICENSE

This project is released into the public domain under the Unlicense.
See the LICENSE file for full legal text.

**NO WARRANTIES OR GUARANTEES OF ANY KIND.**
**USE ENTIRELY AT YOUR OWN RISK.**

## DISCLAIMER

This project is for EDUCATIONAL AND RESEARCH PURPOSES ONLY.
Users must comply with all applicable laws and regulations.
Unauthorized modification of system software may:
- Void warranties
- Violate software licenses
- Compromise system security
- Violate computer fraud and abuse laws

## Overview

This DXE_RUNTIME_DRIVER provides functionality for direct user control at VTL 1 permission level, the new highest permission level in Windows 11.

## References

**References**
- [noahware/hyper-reV](https://github.com/noahware/hyper-reV/tree/main)
- [backengineering/Voyager](https://github.com/backengineering/Voyager/tree/master)
- [SamuelTulach/SecureHack](https://github.com/SamuelTulach/SecureHack)

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

PayloadEntry: (Architecture specific, brief outline)
- GetExitReason
- CheckRaxIsLeaf
- SetR8
- Continue
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

Finally, the payload itself, there are multiple options for handling the payload, we have currently only built the intel version where yet another hack has been used, for some reason within the payload, vmwrite does not work. Like, vmread will read the correct value, but vmwrite will not update it, this can be seen in the comments left over from my headaches. I have chosen to just accept that cpuid will clear four registers (rax, rbx, rcx, rdx) and just put these as argument registers which will be cleared, then we return using the extended registers r8-r15.

Looking for another way is just really not worth the time, this method works, at worse, its more detectable I guess, there is currently no detection for this kind of vector so it doesn't really detract from it at all anyway.

Overall, this project is now classed as a success on intel x64 system (Tested on nixos/kvm(nested)/libvirt).
