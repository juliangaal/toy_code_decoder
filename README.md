# Not QR Code
rotation invariant, experimental codes in this style:

<p float="left">
  <img src="./tests/pics/rect_bw_16_show_off.jpg" width="400" />
</p>

#### Properties
* thick bar used to calculate absolute rotation
* x,y coordinate encoding: 
  * binary representation: big rects represent 1s, small rects represent 0s
  * Once properly rotated (thick bar in the east), rects above middle line represent x coordinate, below y coordinate

#### Dependencies
* OpenCV 4.0
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
