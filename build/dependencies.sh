#!/bin/bash

sudo apt-get update
# Install cmake
export CMAKE_VERSION=3.20.1
wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.tar.gz
tar -zxf cmake-$CMAKE_VERSION.tar.gz
cd cmake-$CMAKE_VERSION
sudo ./bootsrap
make
make install
cmake --version
cd ..
# install other dependencies
sudo apt-get install -y libc++-dev libc++abi-dev bsdtar libedit-dev