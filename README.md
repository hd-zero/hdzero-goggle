# HDZero Goggle Firmware

## Environment Setup

The firmware can either be built in a [devcontainer](https://containers.dev/) or natively on a linux machine.

Note: decompressing the repository in Windows system may damage some files and prevent correct builds.

### Devcontainer Setup

This repository supports the [vscode devcontainer](https://code.visualstudio.com/docs/devcontainers/containers) integration.
To get started, install docker, vscode and the devcontainer extension.
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
~/hdzero-goggle/build$ make clean all -j $(nproc)
```

The firmware is generated as hdzero-goggle/out/HDZERO_GOGGLE-x.x.x.bin
Where x.x.x is the OTA_VER.RX_VER.VA_VER

### Building the firmware using nix

The nix build system can be used to build the firmware on any linux system.  
Make sure that nix [is installed](https://nixos.org/download/), and the [flakes feature](https://wiki.nixos.org/wiki/Flakes) is enabled.  
No bootstrapping or installation of any tools is required.

Use this command to build the firmware

```shellSession
nix build .#goggle-app
```

After this succeeds, the firmware can be found under `./result` in the current directory.


## Loading the Firmware

There following methods can be used to load the firmware:
- flash it via the goggle menu
- temporarily execute it from an sdcard
- temporarily execute it via ssh

### Executing firmware via sdcard

On boot, the goggles check the root of the sdcard for a `develop.sh` file. If it exists, it gets executed. This can be used to launch a custom build of the `HDZGOGGLE` executable.

SD Card File Hierarchy:

```
/develop.sh
/HDZGOGGLE
```

`develop.sh` to launch HDZGOGGLE from sdcard or fall back to builtin executable if not found:

```bash
#!/bin/sh

# Load via SD Card if found
if [ -e /mnt/extsd/HDZGOGGLE ]; then
	/mnt/extsd/HDZGOGGLE &
else
	/mnt/app/app/HDZGOGGLE &
fi
```

### Execute firmware via ssh

Use the script `./utilities/ssh-deploy.sh` inside this repo to temporarily run a custom build of the `HDZGOGGLE` executable on the goggles.

Setup:

1. Have the [HDZero Expansion Module](https://www.hd-zero.com/product-page/hdzero-goggle-expansion-module) installed, as this provides wifi capabilities
2. Enable the wifi via the goggle menu and connect it to a network which is reachable from the dev machine
3. Enter the wifi menu again on the goggle to find its IP address (for example `192.168.1.5`)
3. Execute `./utilities/ssh-deploy.sh <host> <HDZGOGGLE_binary>`

Example:
```shellSession
./utilities/ssh-deploy.sh root@192.168.1.5 ./build/HDZGOGGLE
```

A powercycle will switch the goggles back to the builtin `HDZGOGGLE`.

## Building the Emulator

Goggle source code can be built natively on the host machine and used for debugging.

### Library required

Requires build-essential tools and SDL2 development libraries (libsdl2-dev for debian) to be already installed.

```
sudo apt-get install build-essential libsdl2-dev
```

### Build and Run

```
~/hdzero-goggle$ mkdir build_emu
~/hdzero-goggle$ cd build_emu
~/hdzero-goggle/build_emu$ cmake .. -DEMULATOR_BUILD=ON -DCMAKE_BUILD_TYPE=Debug
~/hdzero-goggle/build_emu$ make -j $(nproc)
~/hdzero-goggle/build_emu$ ./HDZGOGGLE
```

### Emulator Keys

`a` = right button press
`w` = wheel up
`s` = wheel down
`d` = wheel center press
Use `F11` to toggle full screen where applicable.

## Support and Developer Channels

Join the official Discord server here:

https://discord.gg/kGsnEDMb2V

Or the official Facebook group:

https://www.facebook.com/groups/hdzero
