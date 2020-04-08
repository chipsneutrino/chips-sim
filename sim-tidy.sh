#! /bin/bash

CURRENTDIR=$(pwd)

cd $CHIPSSIM
make clean
rm ./cmake_install.cmake
rm ./CMakeCache.txt
rm -r ./CMakeFiles
rm ./Makefile
rm ./example_output.root
rm ./geofile.txt

cd $CURRENTDIR