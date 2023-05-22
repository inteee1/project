import smbus
from Motor_Lib import PWM
from time import sleep
import socket
import threading
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
SERVO1 = 3
SERVO2 = 4
HOST = 'localhost'
PORT = 9999
def received_message(clnt):
    while True: 
        data = clnt.recv(1024).decode(encoding='utf-8')
        data = data.strip()
        if data == "문이 열립니다.":
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

with socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM, proto=0) as clnt:
    try:
        clnt.connect((HOST, PORT))
      
        t2 = threading.Thread(target=received_message, args=(clnt,))
      
        t2.start()
      
        t2.join()
    except KeyboardInterrupt:
        print('Keyboard interrupt')
    finally:
        GPIO.cleanup()