#!/usr/bin/env bash
set -e # exit on first fail
sudo add-apt-repository -y universe
sudo apt-get update -y
sudo apt-get install -y git cmake libboost-all-dev 

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
sudo apt-get install -y libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

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
