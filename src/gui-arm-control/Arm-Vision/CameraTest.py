"""
Go to https://www.pyimagesearch.com/2016/10/24/ubuntu-16-04-hot-to-install-opencv/
for instructions on how to install the openCV library on Ubuntu.
Note that if you are using a version of ubuntu newer than 16.04 you may run into a few minor
issues while trying to install dependencies.

Issues encountered so far and potential fixes (that worked for me):
1. libpng-12-dev:
- sudo apt-get install libpng-dev

2. libjapser-dev:
- sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"
- sudo apt update
- sudo apt install libjasper1 libjasper-dev

3. libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
- skipping this step

Go to http://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_setup/py_setup_in_windows/py_setup_in_windows.html
to install openCV library on windows
"""
# This code will open a window and display a camera feed from a webcam/usb camera

import numpy as np
import cv2
""""
 To capture video we need to create a video capture object.
 It's argument can take either be the device index device index or the name of a video file. 
 The device index is just the number to specify which camera to use. 
 If you have one camera connected, just pass 0. If you have more
 than one camera then you can select the second camera by passing 1
"""
cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Display the resulting frame
    cv2.imshow('frame',gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
