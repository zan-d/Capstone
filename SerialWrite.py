import serial
ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM4'

ser.open()

ser.write(b"100\n")

#while True:
h1=ser.readline()
if h1:
	g3=int(h1);
	g3=g3+5;
	print(g3)






