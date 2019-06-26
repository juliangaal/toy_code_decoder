#!/usr/bin/env python3
import notqrcode_py as qr
import sys

code = qr.NotQRCodeDecoder("../tests/pics/rect_bw_16_0_xlarge.jpg")

code.calculateKeypoints()
orientation = code.calculateOrientation()
print(orientation.error)
    # if orientation.error is not 2:
    #     print("Can't calculate orientation: {}".format(orientation.error))
    #     sys.exit(1)

code.rotateKeypoints(qr.Degrees(orientation.value))
result = code.decode()
# if result.error is not 0:
#     print("Can't decode: {}".format(result.error))
#     sys.exit(1)

print("Decoded with result: {} and code {}".format(result.error, result.value))