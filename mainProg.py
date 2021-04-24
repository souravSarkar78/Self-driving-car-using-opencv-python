import cv2
import numpy as np
from region import *
from imutils.video import WebcamVideoStream
import serial

arduino = serial.Serial('/dev/ttyACM0', 9600)


x0, y0, x1, y1, x2, y2, x3, y3 = 8,35, 67,35, 4,41 ,72,41
p1 = np.float32([[x0, y0], [x1, y1], [x2, y2], [x3, y3]] )
p2 = np.float32([[0, 0], [80, 0], [0, 45], [80, 45]])

#define region of regionOfInterest

roi_vertices= [[0, 35], [0, 15], [80, 35], [80, 15]]
ro = regionOfInterest()

src = 0
cam = WebcamVideoStream(src).start()


l = 0
r = 0
val = 0
manual = True
s=0

def processed(frame):
    r = frame[:, :, 1]
    _, r = cv2.threshold(r, 180, 255, 0)
    return r


    
while True:
    arduino.flushInput()
    frame2 = cam.read()
    frame3 = cv2.resize(frame2, (320, 240))
    frame = cv2.resize(frame3, (80, 45))
    
    get = cv2.getPerspectiveTransform(p1, p2)
    prr = cv2.warpPerspective(frame, get, (80, 45))

    
    roi1 = ro.roi(prr, np.array([roi_vertices], np.int32))

    mask = processed(roi1)
    
    ############3

    
    _, cont, h = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)  
    
    cv2.drawContours(roi1, cont, -1, (255,0,255), 2)

    cv2.line(prr, (40, 0), (40, 45), (255, 0, 255), 1 )


    for i in range (len(cont)):
        x, y, w, h = cv2.boundingRect(cont[i])

        if x+(w/2) > 32:
            colour = (0, 255, 0)
            r = int(x+(w/2))
        else:
            colour = (255, 0, 5)
            l = int(x+(w/2))


        actual_val = int(((r-l)/2)+l)
        
        
        cv2.circle(prr, (int(actual_val), 20), 1, (0, 255, 0), -1)
   
        cv2.rectangle(prr, (x, y), (x+w, y+h), colour, 2 )

        actual_val += 38

        if actual_val > 90:
            actual_val = 90
        if actual_val < 65:
            actual_val = 65

        
        if manual == False:
            print(chr(actual_val))
            arduino.write(chr(actual_val).encode())   #Sending data to arduino
        
    cv2.imshow('trr', prr)
    cv2.imshow('frame2', frame3)
    #cv2.imshow('frame', frame)
    cv2.imshow('roi', roi1)
    #cv2.imshow('mask', mask)
    key = cv2.waitKey(1)
    #print(key)
    
    if key == 32:
        arduino.write('r'.encode())
        if manual == False:
            
            arduino.write('l'.encode())
        break


    #Manual Key commands 
        
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
    elif key == 110:
        arduino.write('n'.encode())
    elif key == 109:
        arduino.write('m'.encode())
    elif key == 111:
        arduino.write('o'.encode())
    elif key == 122:
        arduino.write('z'.encode())
    elif key == 104: #h
        cv2.imwrite('midpoint.jpg', prr)
        
    
    
    
cv2.destroyAllWindows()  







