#!/usr/bin/python
# -*- coding: utf-8 -*-

import spidev
import sys
from time import sleep

m6675 = spidev.SpiDev(0,0)
m6675.max_speed_hz=1000000

def c_to_f(temp):
    f = ((temp/5)*9)+32
    return f

def readTempC():
    m6675.writebytes([0x00,0x00])
    tempRead = m6675.readbytes(2)
    temp = (tempRead[0] <<8 | tempRead[1]) >> 3
    return temp * 0.25

#while True:
temp = readTempC()
arq = open("file.csv","a")
arq.write(str(temp)+",")
arq.close()
print("termopar "+str(temp))
 

#   print('Thermocouple Temperature: {0:0.3F}°C / {1:0.3F}°F'.format(temp, c_to_f(temp)))
#    sleep(2)
