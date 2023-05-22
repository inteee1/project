import cv2
import smbus
import time
from Motor_Lib import PWM
from time import sleep
import numpy as np
import matplotlib.pylab as plt

SERVO1 = 3
SERVO2 = 4
def motor():
    i2c_bus = smbus.SMBus(1)
    pwm = PWM(i2c_bus)
    pwm.set_frequency(60)
    try:
        for i in range(540, -1, -5):
            pwm.set_duty(SERVO1, i)
            pwm.set_duty(SERVO2, i)
            sleep(0.1)
    except KeyboardInterrupt:
        pass
    i2c_bus.close()


print("얼굴 인증을 시작하겠습니다.")
cap = cv2.VideoCapture(-1)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
if not cap.isOpened():
    print("cap open failed")
    exit()

face_xml = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
eye_xml = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_eye.xml')
counter = 1
is_face_detected = False
while True:
    ret, img = cap.read()
    if not ret:
        print("Can't read cap")
        break
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    faces = face_xml.detectMultiScale(img_gray, 1.3, 5)
    for(x, y, w, h) in faces:
        
        
        cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0 , 0), 2)
        is_face_detected = True
        roi_color = img[y:y+h, x:x+w]
        face_image = img[y:y+h, x:x+w]
        cv2.imwrite(f"face.png", face_image)
        # counter += 1
        # roi_gray = img_gray[y:y+h, x:x+w]
        # eyes = eye_xml.detectMultiScale(roi_gray)
        # for(ex, ey, ew, eh) in eyes:
        #     cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)
    cv2.imshow("Frame", img)
    
    if cv2.waitKey(1) == 27:
        break
    if is_face_detected:
        print("얼굴이 인식되었습니다.")
        protect_img = cv2.imread("protect.jpg")
        face_img = cv2.imread("face.png")
        hist1 = cv2.calcHist([protect_img], [0], None, [256], [0,256])
        hist2 = cv2.calcHist([face_img], [0], None, [256], [0, 256])
        similarity = cv2.compareHist(hist1, hist2, 3) #3 = cv2.HISTOMP_BHATTACHARYYA
        print(similarity)
        if similarity < 0.7:
            print("확인되었습니다. 문이 열립니다.")
            motor()
        #img2 = cv2.imread("face.png")
            cv2.imshow("save", face_image)
            cv2.waitKey(10000)
            time.sleep(10)
            is_face_detected = False
            break
        else:
            print("얼굴이 다릅니다. 종료합니다.")
            break
   


cv2.destroyAllWindows()