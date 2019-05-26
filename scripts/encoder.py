#!/usr/bin/env python3
# C++ Version https://stackoverflow.com/questions/44453088/how-to-convert-c-array-to-opencv-mat
import random
import numpy as np
import cv2

#let's create a 6 x 6 matrix with all pixels in black color
size = 1000
img = np.zeros((size,size,3),np.uint8)

#let's use "for" cycle to change colorspace of pixel in a random way
for x in range(size):
    for y in range(size):
        value = random.randint(0,1)
        if value == 1:
            img[x,y] = [255,255,255]

#save our image as a "png" image
cv2.imwrite("output.png",img)