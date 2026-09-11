from PIL import Image, ImageDraw
import math
import serial, time

totalFrames = 2

w=28
h=26

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

def image2page(b):
	page=[0]*13*4
	for y in range(13):
		page[y]   =b[4*y+3]
		page[y+13]=b[4*y+2]
		page[y+26]=b[4*y+1]
		page[y+39]=b[4*y+0]
	return bytes(page)


s=serial.Serial("/dev/ttyUSB0",1000000)
a=0.0
while True:
	a = a+ 0.2
	if a>math.pi*2:
		a=0
	im=drawSpin(a)
	s.write(b"Magi")
	s.write([totalFrames])
	s.write(image2page(im))
	s.write(image2page(im[52:]))
	s.write([0x0A])

	time.sleep(0.03)


