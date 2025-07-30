# HDZero Goggle/BoxPro Firmware

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

## Building Firmware

In either of the above scenarios the firmware can be built via make.
An appropiate vscode build task ships with this repository as well.

Compiling HDZero Goggles:
```
~/hdzero-goggle$ cd build_goggle
~/hdzero-goggle/build_goggle$ make clean all -j $(nproc)
```

The firmware is generated as hdzero-goggle/build_goggle/out/HDZERO_GOGGLE-x.x.x.bin
Where x.x.x is the OTA_VER.RX_VER.VA_VER

Compiling HDZero BoxPro:
```
~/hdzero-goggle$ cd build_boxpro
~/hdzero-goggle/build_boxpro$ make clean all -j $(nproc)
```

Compiling HDZero Emulator:
```
~/hdzero-goggle$ cd build_emu
~/hdzero-goggle/build_emu$ make clean all -j $(nproc)
```

The firmware is generated as hdzero-goggle/build_boxpro/out/HDZERO_BOXPRO-x.x.x.bin
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

Firmware can be either flashed via goggle menu or alternatively be executed via the SD Card with a custom development script.  An example of this development script is provided below.  The goggles automatically checks to see if the develop.sh script exists in the root of the SD Card and if found develop.sh is then executed.

The following files must be placed in the root of SD Card in this example. This script will then check to see if HDZGOGGLE binary has been found during bootup and if found then executed.

Otherwise, if the HDZGOOGLE binary is not detected, the goggles will continue to load the built-in executable which was previously flashed.

SD Card File Hierarchy:

```
/develop.sh
/HDZGOGGLE
```

Development script (develop.sh):

```
#!/bin/sh

# Load via SD Card if found
if [ -e /mnt/extsd/HDZGOGGLE ]; then
	/mnt/extsd/HDZGOGGLE &
else
	/mnt/app/app/HDZGOGGLE &
fi
```

## Building and Running the Emulator

The goggle's UI can be built and run as an emulator on a host machine for easier development and debugging.

### Prerequisites

-   **On Linux:** You need the essential build tools and SDL2 development libraries.
    ```shell
    sudo apt-get install build-essential libsdl2-dev
    ```
-   **On macOS (ARM):** The emulator runs inside the devcontainer. You need to prepare your Mac to display the UI from the container by installing and configuring **XQuartz**.

### Step 1: Prepare Your Mac for UI Forwarding

This setup is done **once** on your Mac to allow UI from the devcontainer to appear on your desktop.

1.  **Install XQuartz:**
    ```shell
    brew install --cask xquartz
    ```

2.  **Configure XQuartz:** Open XQuartz. In the top menu bar, go to `XQuartz` -> `Settings...` -> `Security` and ensure **"Allow connections from network clients"** is checked. Restart `XQuartz`

3.  **Apply settings from the terminal:**
    ```shell
    defaults write org.xquartz.X11 nolisten_tcp 0
    defaults write org.xquartz.X11 enable_iglx -bool YES
    ```

4.  **Reboot your Mac.**

### Step 2: Build the Emulator

The build process is the same for all platforms, but on macOS it should be done inside the devcontainer

1.  **For macOS users:** Open the project in the devcontainer. When prompted, choose the **"HDZero for ARM64"** configuration.

2.  **Compile the emulator:**
    ```shell
	cd build_emu
    make -j
    ```

### Step 3: Run the Emulator

1.  **On macOS:**
    *   Make sure the **XQuartz** application is running (Ensure your system keyboard layout is set to **English** to avoid known bug, when **XQuartz** uses __only__ locale that was selected before the start)
    *   In a **Mac terminal** (not the devcontainer one), run `xhost +` to temporarily allow all connections. This is needed for the container to connect.

2.  **Inside the devcontainer or your terminal**, run the compiled application:
    ```shell
    ./HDZGOGGLE
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
