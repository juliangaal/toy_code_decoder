#!/usr/bin/env python3
# C++ Version https://stackoverflow.com/questions/44453088/how-to-convert-c-array-to-opencv-mat
import random
import numpy as np
import cv2

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        value = random.randint(0,1)
        if value == 1:
            img[x,y] = [255,255,255]

cv2.imwrite("output1.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        value = random.randint(0,2)
        if value == 1:
            img[x,y] = [255,255,255]
        if value == 2:
            img[x,y] = [128,128,128]

cv2.imwrite("output2.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        if y % 5 == 0:
            img[x,y] = [255,255,255]
    
cv2.imwrite("output3.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        if x % 10 == 0:
            img[x,y] = [255,255,255]

cv2.imwrite("output4.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        if y > size/2:
            img[x,y] = [255,255,255]
        
cv2.imwrite("output5.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        if y > size/2 and y < size/4:
            img[x,y] = [255,255,255]

cv2.imwrite("output6.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        value = random.randint(0,1)
        if value == 1:
            img[x,y] = [255,255,255]

cv2.imwrite("output6.png",img)

size = 1000
img = np.zeros((size,size,3),np.uint8)

for x in range(size):
    for y in range(size):
        value = random.randint(0,1)
        if value == 1:
            img[x,y] = [255,255,255]

cv2.imwrite("output7.png",img)
