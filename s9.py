from Xlib import display, X
from PIL import Image, ImageDraw, ImageOps
import serial, time

totalFrames = 8

scale=1

w=28*2
h=26*2

d = display.Display()
root = d.screen().root


def grab_bytes():
	raw = root.get_image(0,0,w,h, X.ZPixmap, 0xffffffff)
	image = Image.frombytes("RGB", (w, h), raw.data, "raw", "BGRX")
	image = ImageOps.invert(image)
	#image.show()
	image = (image.resize((w,h))
		.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
		.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
		.convert("1", dither=1))
	return image.tobytes()





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
while True:
	im=grab_bytes()
	s.write(b"Magi")
	s.write([totalFrames])
	s.write(image22page(im[ 91*3: ]))
	s.write(image22page(im[ 91*2: ]))
	s.write(image22page(im[ 91: ]))
	s.write(image22page(im[ 0: ]))
	s.write([0x0A])

	time.sleep(0.02)


