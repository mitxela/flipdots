import serial, time
s=serial.Serial("/dev/ttyUSB0",1000000)

totalFrames = 2;

#for i in range(255):
while True:

	s.write(b"Magi")
	s.write([totalFrames])
	s.write([0x00,0x00,0x00,0x00]*13)
	s.write([0xAA,0xAA,0x55,0x55]*13)
	s.write([0x0A])

	time.sleep(0.5)

	s.write(b"Magi")
	s.write([totalFrames])
	s.write([0xAA,0xAA,0x55,0x55]*13)
	s.write([0x00,0x00,0x00,0x00]*13)
	s.write([0x0A])

	time.sleep(0.5)
