#!/bin/bash

sudo apt-get update
# Install gcc and other compilers
sudo apt-get install build-essential
#sudo apt install gcc-8 g++-8 gcc-9 g++-9 gcc-10 g++-10
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-9 g++-9
# install other dependencies
sudo apt-get install -y libc++-dev libc++abi-dev bsdtar libedit-dev
# Install make
sudo apt install make
# Install cmake
#export CMAKE_VERSION=3.20.1
#wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.tar.gz
#tar -zxf cmake-$CMAKE_VERSION.tar.gz
#cd cmake-$CMAKE_VERSION
#sudo ./bootstrap
#make
#make install
pip install --upgrade cmake
cmake --version
cd ..
