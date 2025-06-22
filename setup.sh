#!/bin/bash
TOOLCHAIN_FILE="armv7-eabihf--musl--stable-2018.02-2.tar.bz2"
TOOLCHAIN_URL="https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/$TOOLCHAIN_FILE"

if [ ! -d toolchain ]; then
  if [ "$1" == "automated_build" ]; then
    echo "Extracting toolchain..."
    mkdir toolchain
    wget -qO- "$TOOLCHAIN_URL" | tar xj --strip-components=1 -C toolchain
  fi

if [ ! -z "$1" ] && [ "$1" != "automated_build" ]; then
echo "Working directory: `pwd`"
cp $1 .
fi

if [ ! -f "$TOOLCHAIN_FILE" ] && [ "$1" != "automated_build" ]; then
  echo "$TOOLCHAIN_FILE not found."
  while true; do
    read -p "download $TOOLCHAIN_URL? (y/n): " yn
      case $yn in
        [yY]* ) wget "$TOOLCHAIN_URL"; break;; # Accepts 'y', 'Y', 'yes', 'YES', etc.
        [nN]* ) echo "Please run setup.sh with path to toolchain archive eg: "; echo "./setup.sh $TOOLCHAIN_FILE"; exit;; # Accepts 'n', 'N', 'no', 'NO', etc.
        * ) echo "Invalid response. Please enter 'y' or 'n'.";;
      esac
  done
fi
  if [ -f "$TOOLCHAIN_FILE" ]; then  
    echo "Using toolchain `pwd`/$TOOLCHAIN_FILE"
    if [ ! -d toolchain ]; then
      echo "Extracting toolchain..."
      mkdir toolchain
      tar -xvjf $TOOLCHAIN_FILE --strip-components=1 -C toolchain
    fi
  fi
fi

rm -rf build_*

echo
echo "**********************************************"
echo "Preparing HDZero Goggle Build Environment....."
echo "**********************************************"
cmake . -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchain/share/buildroot/toolchainfile.cmake -Bbuild_goggle

echo
echo "**********************************************"
echo "Preparing HDZero BoxPro Build Environment....."
echo "**********************************************"
cmake . -DHDZ_BOXPRO=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchain/share/buildroot/toolchainfile.cmake -Bbuild_boxpro

echo
echo "****************************************"
echo "*** HDZero Goggle Build Instructions ***"
echo "****************************************"
echo "cd build_goggle; make -j"

echo
echo "****************************************"
echo "*** HDZero BoxPro Build Instructions ***"
echo "****************************************"
echo "cd build_boxpro; make -j"
