#/usr/bin/env bash

/bin/bash install.sh

mkdir -p build && cd build
cmake -DCOMPILE_TEST=ON -DENABLE_AUTO_TEST=ON -DCMAKE_BUILD_TYPE=Release .. && make -j3