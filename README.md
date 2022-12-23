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
