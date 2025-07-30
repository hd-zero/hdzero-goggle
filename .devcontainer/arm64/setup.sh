#!/bin/bash
rm -rf build_*

echo
echo "**********************************************"
echo "Preparing HDZero Goggle Build Environment....."
echo "**********************************************"
cmake . -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Release -DDEVCONTAINER_ARM=ON -Bbuild_goggle

echo
echo "**********************************************"
echo "Preparing HDZero BoxPro Build Environment....."
echo "**********************************************"
cmake . -DHDZ_BOXPRO=ON -DCMAKE_BUILD_TYPE=Release -DDEVCONTAINER_ARM=ON -Bbuild_boxpro

echo
echo "**********************************************"
echo "Preparing HDZero Emulator Build Environment..."
echo "**********************************************"
cmake . -DEMULATOR_BUILD=ON -DCMAKE_BUILD_TYPE=Debug -DHDZ_GOGGLE=ON -DHDZ_BOXPRO=OFF -Bbuild_emu

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

echo
echo "****************************************"
echo "***** HDZero Emulator Instructions *****"
echo "****************************************"
echo "cd build_emu; make -j"