#!/usr/bin/env python3
#pip3 install cobs pyserial
from cobs import cobs
from time import sleep
import serial

TEST_TIME=1000

ser = serial.Serial('/dev/ttyACM0', 9600,timeout=1) #put your COM port here

percent = 0
goingDown=False

colors = [None,[1,10,1],[1,1,10]]
modes=[None,'travel','regular','reverse','rainbow']

for player in range(1,3):
	print("player "+str(player))
	for mode in range(1,4):
		print("Test mode "+str(mode)+ " "+modes[mode])
		#print('')
		for i in range(TEST_TIME):
			LEDdata = bytearray([mode,player,1,colors[player][1],colors[player][2],percent])
			#print(str(LEDdata),end='\r')
			#encoded = cobs.encode(bytearray(LEDdata))
			ser.write(LEDdata)
			#ser.flush()
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
			
	print("Test mode 4")
	for i in range(int(TEST_TIME/2)):
		LEDdata = bytearray([4,player,1,1,1,percent])
		#print(LEDdata)
		#encoded = cobs.encode(bytearray(LEDdata))
		ser.write(LEDdata)
		#ser.flush()
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
		sleep(0.02)
	for i in range(int(TEST_TIME/2)):
		LEDdata = bytearray([4,player,1,1,1,255])
		ser.write(LEDdata)

		sleep(0.015)
print('done')
