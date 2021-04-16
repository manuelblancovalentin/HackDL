#!/bin/bash

sudo apt-get update
# Install gcc and other compilers
sudo apt-get install build-essential
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-9 g++-9
# install other dependencies
sudo apt-get install -y libc++-dev libc++abi-dev bsdtar libedit-dev
# Install make
sudo apt install make
pip install --upgrade cmake
cmake --version
cd ..
