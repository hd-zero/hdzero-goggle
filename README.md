# HDZero Goggle Firmware

## Environment Setup

The firmware can either be built in a [devcontainer](https://containers.dev/) or natively on a linux machine.

### Devcontainer Setup

This repository supports the [vscode devcontainer](https://code.visualstudio.com/docs/devcontainers/containers) integration.  
To get started install docker, vscode and the decontainer extension.  
A [prompt](https://code.visualstudio.com/docs/devcontainers/create-dev-container#_add-configuration-files-to-a-repository) to reopen this repository in a container should appear.

### Native Setup

CMake is required for generating the build files.  
A bash script is supplied to take care of the bootstrap process:

```
~/hdzero-goggle$ ./setup.sh
```

## Building the Firmware

In either of the above scenarios the firmware can be built via make.  
An appropiate vscode build task ships with this repository as well.

```
~/hdzero-goggle$ cd build
~/hdzero-goggle/build$ make clean all
```

The firmware is generated as ~/hdz_goggle/code/out/HDZERO_GOGGLE-x.x.x.bin
Where x.x.x is the OTA_VER.RX_VER.VA_VER

## Loading the Firmware

Firmware can be either flashed via goggle menu or alternatively be executed via the SD Card with a custom development script.  An example of this development script is provided below.  The goggles automatically checks to see if the develop.sh script exists in the root of the SD Card and if found develop.sh is then executed.

The following files must be placed in the root of SD Card in this example. This script will then check to see if HDZGOGGLE binary has been found and if found then executed. The standard output for logging will be written to a file called HDZGOOGLE.dev.log

Otherwise, if the HDZGOOGLE binary is not detected, the goggles will continue to load the built-in executable which was previously flashed.

SD Card File Hierarchy:

```
/develop.sh
/HDZGOOGLE
```

Development script (develop.sh):

```
#!/bin/sh

# Load via SD Card if found
if [ -e /mnt/extsd/HDZGOGGLE ]; then
	/mnt/extsd/HDZGOGGLE > /mnt/extsd/HDZGOGGLE.dev.log 2>&1 &
else
	/mnt/app/app/HDZGOGGLE &
fi
```

## Building the Emulator

Goggle source code can be built natively on the host machine and used for debugging.
Requires build-essential tools and SDL2 development libraries (libsdl2-dev for debian) to be already installed.

```
~/hdzero-goggle$ mkdir build_emu
~/hdzero-goggle$ cd build_emu
~/hdzero-gogglei/build$ cmake .. -DEMULATOR_BUILD=ON -DCMAKE_BUILD_TYPE=Debug
~/hdzero-gogglei/build$ make -j
~/hdzero-gogglei/build$ ./HDZGOGGLE
```

Emulator Keys:  
a = right button press  
w = wheel up  
s = wheel down  
d = wheel center press  
Use F11 to toggle full screen where applicable.

## Support and Developer Channels

Join the official Discord server here:

https://discord.gg/kGsnEDMb2V


Or the official Facebook group:

https://www.facebook.com/groups/hdzero
