# Initial setup

## 1. Extract supporting libraries and head files

```
~/hdz_goggle/code$ tar xzvf linux-4.9.tar.gz
~/hdz_goggle/code$ cat media.tar.gzaa media.tar.gzab > media.tar.gz
~/hdz_goggle/code$ tar xzvf media.tar.gz
~/hdz_goggle/code$ tar xzvf toolchain.tar.gz
```

## 2. Setup cmake environment

```
~/hdz_goggle/code$ ./setup.sh
```

# Build code

```
 ~/hdz_goggle/code/$ cd build
 ~/hdz_goggle/code/build$ make clean all
```

the firmware is generated on ~/hdz_goggle/code/out


# Building in a container

After step 1 in the Initial Setup section above...

## 1. Start a container for building

Run an ubuntu container, mounting the current directory (the git repository) as `/src` in the container and install build prerequistires
```
docker run -v `pwd`:/src -it ubuntu /bin/bash
```
in the container
```
# apt-get update
# apt-get install cmake build-essential
```

## 2. Setup cmake environment

```
# cd /src
# ./setup.sh
```

## 3. Build code (in the container)

```
/src# cd build
/src/build# make clean all
```

the firmware is generated on ~/hdz_goggle/code/out
