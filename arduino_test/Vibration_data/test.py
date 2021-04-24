import cv2
import numpy as np
from region import *
from imutils.video import WebcamVideoStream
import serial

arduino = serial.Serial('/dev/ttyACM0', 9600)




src = 0
cam = WebcamVideoStream(src).start()



    
while True:
    arduino.flushInput()
    frame2 = cam.read()
    frame3 = cv2.resize(frame2, (320, 240))

    
    
    cv2.imshow('frame2', frame3)

    key = cv2.waitKey(1)
    #print(key)
    
    if key == 32:
        arduino.write('r'.encode())
        break

    elif (key == 119):
        arduino.write('w'.encode())
    elif key == 114:
        arduino.write('r'.encode())
    elif key == 108:
        if manual == False:
            manual = True
        else:
            manual = False
            
        arduino.write('l'.encode())
    elif key == 97:
        arduino.write('a'.encode())
    elif key == 100:
        arduino.write('d'.encode())
    elif key == 115:
        arduino.write('s'.encode())
    elif key == 101:
        arduino.write('e'.encode())
    
    
    
cv2.destroyAllWindows()  







