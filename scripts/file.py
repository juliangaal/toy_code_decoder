#!/usr/bin/env python3
import notqrcode_py as qr
import sys

def run():
    code = qr.NotQRCodeDecoder.file("../tests/pics/rect_bw_16_neg90_xlarge.jpg", 0) # 0 is grayscale

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

if __name__ == '__main__':
    run()