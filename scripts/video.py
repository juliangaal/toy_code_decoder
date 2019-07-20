#!/usr/bin/env python3
import notqrcode_py as qr
import cv2
import sys

def run():
    params = qr.VideoParams()
    params.width = 640
    params.height = 480
    params.frame_rate = 10
    params.id = 0
    code = qr.NotQRCodeDecoder.video(params)

    while True:
        code.nextFrame()
        code.calculateKeypoints()
        orientation = code.calculateOrientation()
        if orientation.error != 0:
            print("Can't calculate orientation: {}".format(orientation.error))
            continue

        code.rotateKeypoints(qr.Degrees(orientation.value))
        result = code.decode()
        if result.error != 0:
            print("Can't decode: {}".format(result.error))
            continue

        print("Decoded with orientation {:4.2f} deg and code {}".format(orientation.value, result.value))

if __name__ == '__main__':
    run()