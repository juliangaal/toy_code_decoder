#!/usr/bin/env bash
set -e # exit on first fail
sudo add-apt-repository -y universe
sudo apt-get update -y
sudo apt-get install -y build-essential git cmake libboost-all-dev python3-pip

loc=$PWD
mkdir -p $HOME/install_stuff
cd $HOME/install_stuff
if [ ! -d "fmt" ]; then
  git clone https://github.com/fmtlib/fmt.git
fi
cd fmt && mkdir -p build && cd build 
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
sudo make install

cd $HOME/install_stuff
if [ ! -d "Catch2" ]; then
  git clone https://github.com/catchorg/Catch2.git
fi
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build/ --target install

# install opencv
cd $HOME/install_stuff
sudo apt-get install -y gcc g++ gtk2.0 ffmpeg python3-numpy python-numpy
# gstreamer
#sudo apt-get install -y gstreamer1.0*

if [ ! -d "opencv" ]; then
  git clone https://github.com/opencv/opencv.git
fi
cd opencv && mkdir -p build && cd build
cmake -DWITH_EIGEN=OFF -DCMAKE_BUILD_TYPE=Release ..
make -j4 
sudo make install

# install python stuff
cd $loc
pip3 install -r requirements.txt

# run decoder tests
cd $HOME && rm -rf $HOME/install_stuff
cd $loc && mkdir -p build && cd build 
cmake -DAUTO_TEST=ON -DCMAKE_BUILD_TYPE=Release .. && make -j3
