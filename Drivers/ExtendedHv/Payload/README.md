# ExtendedHV Payload

## Overview

To work with code inside of the hypervisor we have to be a little hacky, we need position independent code that we can run inside of the hypervisor since we can not jump out to our code and have it run. So, we build a separate payload, compiled separately away from the main driver, then, we copy the bytes straight into that drivers code.

There are multiple optimizations we are looking to make to this in the future, one such idea is to have the driver load this payload into its memory dynamically to allow for custom payload, and isolated payload development, but, we are still in the early stages and this is the best usecase right now.

The makefile is specific to linux (and wsl2) systems, thus, if you are on windows you should install wsl2, and if you are on linux you will be looking to ensure you have the basic gnu build tools available.

We compile this without any UEFI features, which should be obvious, and we require no dynamic linking of any kind.


