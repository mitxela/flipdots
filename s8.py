from PIL import Image, ImageDraw
import math
import serial, time

totalFrames = 8

w=28*2
h=26*2

def drawSpin(angle):
	im = Image.new("1", (w,h))
	d = ImageDraw.Draw(im)

	x=w/2
	y=h/2
	c=math.cos(angle)*100
	s=math.sin(angle)*100
	d.polygon( [ x+c, y+s, x+s,y-c, x-c,y-s ] ,fill=1)

#	im.show()
	return im.tobytes()

def image22page(b):
	b = b + bytes([0]*52) # end padding

	page1=[0]*13*4
	page2=[0]*13*4

	for y in range(13):
		page1[y]   =b[7*y+3]
		page1[y+13]=b[7*y+2]
		page1[y+26]=b[7*y+1]
		page1[y+39]=b[7*y+0]

		page2[y]   =((b[7*y+6]<<4)&0xFF) + ((b[7*y+7]>>4)&0xFF)
		page2[y+13]=((b[7*y+5]<<4)&0xFF) + ((b[7*y+6]>>4)&0xFF)
		page2[y+26]=((b[7*y+4]<<4)&0xFF) + ((b[7*y+5]>>4)&0xFF)
		page2[y+39]=((b[7*y+3]<<4)&0xFF) + ((b[7*y+4]>>4)&0xFF)
	return bytes(page2)+bytes(page1)


s=serial.Serial("/dev/ttyUSB0",1000000)
a=0.0
while True:
	a = a+ 0.2
	if a>math.pi*2:
		a=0
	im=drawSpin(a)
	s.write(b"Magi")
	s.write([totalFrames])
	#for i in range(4):
	s.write(image22page(im[ 91*3: ]))
	s.write(image22page(im[ 91*2: ]))
	s.write(image22page(im[ 91: ]))
	s.write(image22page(im[ 0: ]))
	s.write([0x0A])

	time.sleep(0.03)


