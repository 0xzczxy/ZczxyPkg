# ZczxyPkg

**Warning: The list of contributors is apparently automatically compiled from commits, I did not know this when I was just putting in random names, its only me working on this project currently and I should probably figure out a way to strip those artifacts.**

## Overview

All my UEFI work is currently being done through edk2 GCC5 toolchain, I have seen way to many projects utilizing the MSVC toolchain and as a linux user I just couldn't commit to that. I have tested compilation on both nixos and windows 11 wsl2 Ubuntu 24.04, other operating systems should work as well if you know edk2 to compile and you have glibc installed.

Projects are listed under subdirectories, compilation of the entire package follows.

## Build Instructions

**Clone edk2**
```bash
git clone --recurse-submodules https://github.com/tianocore/edk2.git
cd edk2
```

**Make Base Tools**
```bash
make -C BaseTools
```

**Source edk2 script**
```bash
source edksetup.sh
```

**Create a symbol link to the zczxy package inside the edk2 structure, or copy in, its up to you.**
```bash
ln -s ../ZczxyPkg . # Create a symbolic link
```

**Compile ZczxyPkg**
```bash
build -a X64 -t GCC5 -p ZczxyPkg/ZczxyPkg.dsc
```
_Compiling the package in full is kind of a bad idea, many of these rely on separate payloads which have to be compiled separately, I am still working on the best solution to this so bear with me. For now it should be fine since we only really have one project anyway._

**(Optional) Compile ShellPkg**
```bash
build -a X64 -t GCC5 -p ShellPkg/ShellPkg.dsc
```
_For drivers, they can often be loaded by a shell on consumer hardware. Utilize the shell as your BOOTX64.EFI_

## Virtual Machine Options

My personal development cycle happens with **KVM/Libvirt/virt-manager**, I setup my nixos configuration to support KVM with nested virtualization so I can test windows 11 with the newest security features, this would be my personal choice for anyone on a linux system.

If you are on wsl2, you have a problem, unfortunately, the last time windows made a good addition to the kernel was well before I was even born, hyper-v is a joke for virtual machine development, there is no EFI firmware interface, no usb pass-through, and so many more problems, your only option is vmware for nested virtualization support. However, anyone who knows about vmware knows it requires you turn Hyper-V off, windows does not have a great feature like KVM which can support multiple diverse virtual machine structures, windows has hyper-v which is a bigger problem then solver. If you utilize wsl then you have hyper-v on, so you realistically have to choose between the two.

I am sorry for all windows users, I am trying to create better support for MSVC for all the windows users out there, but I just hate the tooling system that exists on windows, switching from Makefile to mate.c in payloads and separate compilation will be considered to try and help out you poor souls.

## Code Style

I will begin work on a STYLE.md, the styling will unfortunately have some limitations, due to the code style already enforced by the edk2 library we are forced to work with a lot of styling choices decided before we even get to write a line, still, I have tried to bring this as close as possible to my normal C style with a few minimal changes of snake_case to PascalCase and camelCase for fitting with the edk2 code structure.

The major section is just defining extern functions, this may seem dumb with header files but its kind of like a `use::...;` statement, rust is still too immature (and I would say too bulky but look at edk2) so there is still some work to do on creating in rust for EFI. I am not particually the biggest fan of rust mainly due to the standard library, but, I do like the tagged union approach and the error handling ideas. Either way, sticking to C, we define imports explicitily, we define static functions for private use, and in the future it may be worth utilizing const more to make an immutable style more prominent.

I am not currently accepting pull requests or any open source development work, this is a personal project currently developing for educational purposes.



