import socket
import threading
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
leds = [23, 24, 25, 1]
for i in leds:
    GPIO.setup(i, GPIO.OUT)
    pass
HOST = 'localhost'
PORT = 9999

send_message:str = ''

def sending_message(clnt):
    while True: 
        if GPIO.input(leds[0]):
            pass
            
        elif GPIO.input(leds[1]):
            pass

def received_message(clnt):
    while True: 
        data = clnt.recv(1024).decode(encoding='utf-8')
        data = data.strip()
        print(f'received_data : {data}')
        if data == '패배하였습니다.':
            GPIO.output(leds[0], GPIO.HIGH)
            print('LED 1 ON')
            
            pass
        elif data == '[영희] : led1_off':
            GPIO.output(leds[0], GPIO.LOW)
            print('LED 1 OFF')
            pass
        elif data == '패배하였습니다.':
            GPIO.output(leds[1], GPIO.HIGH)
            print('LED 2 ON')
            pass
        elif data == '[영희] : led2_off':
            GPIO.output(leds[1], GPIO.LOW)
            print('LED 2 OFF')

with socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM, proto=0) as clnt:
    try:
        clnt.connect((HOST, PORT))
        t1 = threading.Thread(target=sending_message, args=(clnt,))
        t2 = threading.Thread(target=received_message, args=(clnt,))
        t1.start()
        t2.start()
        t1.join()
        t2.join()
    except KeyboardInterrupt:
        print('Keyboard interrupt')
    finally:
        GPIO.cleanup()
