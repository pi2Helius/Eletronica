#!/usr/bin/env python
import time
import serial
import sys

#Configurando comunicação serial
ser = serial.Serial(
        port='/dev/serial0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

i=0

while (i==0):

        line=str(ser.readline())        #Lê linha por linha do arquivo onde os dados são escritos
        # Serarando os dados recebidos de modo a escrever a localização em termos de latitude e longitude
        if line.startswith("b'$GPRMC"):
                line = line.strip()
                #print (line)
                dados = line.split(',')
                latitude = dados[3]
                graus_lat = latitude[0]+latitude[1]
                longitude = dados[5]
                graus_long = longitude[0]+longitude[1]+longitude[2]
                coordenadas = graus_lat+latitude[2:]+ dados[4]+","+graus_long+longitude[3:]+dados[6]
                print(coordenadas)
                print(graus_lat+latitude[2:]+ dados[4]+",")
                print(graus_long+longitude[3:]+dados[6])

                #Escrevendo as coordenadas no arquivo
                arq = open("file.csv","a")
                arq.write(graus_lat+latitude[2:]+ dados[4]+",")
                arq.write(graus_long+longitude[3:]+dados[6])
                arq.close()

                ser.close()
                i=i+1
