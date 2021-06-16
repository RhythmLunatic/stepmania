#!/usr/bin/env python3
#pip3 install cobs pyserial
from cobs import cobs
from time import sleep
import serial

ser = serial.Serial('/dev/ttyACM0', 9600,timeout=1) #put your COM port here

percent = 0
goingDown=False

for i in range(1000):
	LEDdata = bytearray([2,1,1,1,1,percent])
	print(LEDdata)
	#encoded = cobs.encode(bytearray(LEDdata))
	ser.write(LEDdata)
	ser.flush()
	#print(ser.read(10))
	#print(percent)
	if goingDown:
		percent=percent-1;
		if percent==1:
			goingDown=False
	else:
		percent=percent+1
		if percent>=255:
			goingDown=True
	sleep(0.01)
