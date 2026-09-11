import serial, time
s=serial.Serial("/dev/ttyUSB0",1000000)

totalFrames = 1;

def sendFrame(p):
	s.write(b"Magi")
	s.write([totalFrames])
	s.write([p]*13*4)
	s.write([0x0A])


while True:

	x=0
	for i in range(8):
		x |= (1<<i)
		sendFrame(x);
		time.sleep(0.05)

	time.sleep(0.5)

	for i in range(8):
		x &= (~(1<<i))&0xFF
		sendFrame(x);
		time.sleep(0.05)

	time.sleep(0.5)
#	sendFrame(0xFF)
#	time.sleep(0.5)
#	sendFrame(0)
#	time.sleep(0.5)

