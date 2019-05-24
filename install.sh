#!/usr/bin/env bash
set -e # exit on first fail
sudo DEBIAN_FRONTEND=noninteractive add-apt-repository -y universe
sudo apt-get update -y
sudo apt-get install -y build-essential git cmake libboost-all-dev 

loc=$PWD
mkdir -p $HOME/install_stuff
cd $HOME/install_stuff
if [ ! -d "fmt" ]; then
  git clone https://github.com/fmtlib/fmt.git
fi
cd fmt && mkdir -p build && cd build 
cmake ..
make -j4
sudo make install

# install opencv
cd $HOME/install_stuff
sudo apt-get install -y gcc g++ gtk2.0 ffmpeg python3-numpy python-numpy
# gstreamer
#sudo apt-get install -y gstreamer1.0*

if [ ! -d "opencv" ]; then
  git clone https://github.com/opencv/opencv.git
fi
cd opencv && mkdir -p build && cd build
cmake ..
make -j4 
sudo make install

cd $HOME && rm -rf $HOME/install_stuff
cd $loc && mkdir -p build && cd build 
cmake .. && make -j2
