# Not QR Code
Experimental decoder for codes in this style:

<p float="left">
  <img src="./tests/pics/rect_bw_16_show_off.jpg" width="400" />
</p>

#### Properties
* thick bar used to calculate absolute rotation
* x,y coordinate encoding: 
  * binary representation: big rects represent 1s, small rects represent 0s
  * Once properly rotated (thick bar in the east), rects above middle line represent x coordinate, below y coordinate
* e.g. the picture above will decode to orientation: 0, coordinate: (165, 165)

#### Dependencies
* OpenCV 4.0
* `pyhton(3)-dev` (for python bindings)
* [fmt](https://github.com/fmtlib/fmt) (for tests)
* [Catch](https://github.com/catchorg/Catch2) (for tests)

![CircleCI](https://img.shields.io/circleci/build/github/juliangaal/notqrcode.svg) for ubuntu 16.04, clone this repo and run the [installation script](./install.sh) with `bash scripts/install.sh` to install all necessary components, if they aren't on your system

#### Install
```bash
git clone --recursive https://github.com/juliangaal/notqrcode/
cd notqrcode && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

#### Install Python bindings

By default, the cmake variable `PY_GEN_PATH` is set to `scripts/`. The compiled library will placed there and can only be run from within that directory, unless specified in a different directory. e.g.
`cmake -DPY_GEN_PATH=/some/path`

The easiest way to use the generated library from *anywhere* is to add the output path `PY_GEN_PATH` to `PYTHONPATH`
```bash
PYTHONPATH="${PYTHONPATH}:/path/to/<PY_GEN_PATH>"
export PYTHONPATH
```
or change `PY_GEN_PATH` to an existing path in `$PYTHONPATH`, e.g. `/usr/local/lib/python-<version>/dist-packages`

To run the compiled c++ lib from python code, in `PY_GEN_PATH`: run 
``` bash
python-<version you compiled against> <file>.py
```

#### Test
* setting `-DCOMPILE_TEST=ON` will compile tests and expose target `test` in the cmake Makefile. You can then either run `make test` in the build directory, or `ctest --verbose` for verbose output in case of test failures.
* setting `-DENABLE_AUTO_TEST=ON` will run unit test on every build, if enabled with above command

### Use with C++
Manual mode
```cpp
#include <fmt/format.h>
#include <notqrcode/notqrcode_decoder.hpp>

int main(void) {
    notqrcode::NotQRCodeDecoder decoder(filename);

    decoder.calculate_keypoints(Draw::YES);
    auto orientation = decoder.calculate_orientation(Draw::YES);
    if (orientation.error != Error::None)
        return 1;

    decoder.rotate_keypoints(util::units::Degrees(orientation.val));
    auto decoded_point = decoder.decode();
    if (decoded_point.error != Error::None)
        return 1;

    fmt::print("Rotation: {}\n", orientation.val);
    fmt::print("Decoded: ({},{})\n", decoded_point.val.x, decoded_point.val.y);

    return 0;
}
```
### Use with Python
Manual mode
```python3
#!/usr/bin/env python3
import notqrcode_py as qr
import sys

code = qr.NotQRCodeDecoder("../tests/pics/rect_bw_16_neg90_xlarge.jpg")

code.calculateKeypoints()
orientation = code.calculateOrientation()
if orientation.error != 0:
    print("Can't calculate orientation: {}".format(orientation.error))
    sys.exit(1)

code.rotateKeypoints(qr.Degrees(orientation.value))
result = code.decode()
if result.error != 0:
    print("Can't decode: {}".format(result.error))
    sys.exit(1)

print("Decoded with orientation {:4.2f} deg and code {}".format(orientation.value, result.value))
```

#### Use with cmake
```cmake
cmake_minimum_required(VERSION 3.1)
project(test)

find_package(NotQRCode REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE NotQRCode::notqrcode)
```
