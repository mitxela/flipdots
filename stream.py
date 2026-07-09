from Xlib import display, X
from PIL import Image, ImageDraw, ImageOps
import sys, serial, time

if len(sys.argv) == 3:
	fullscreen = 1 sys.argv[1]=='1' else 0
	dither = 0 sys.argv[2]=='1' else 0
else:
	print("Usage: ",sys.argv[0],"fullscreen dither")
	exit()

totalFrames = 8

w=28*2
h=26*2

d = display.Display()
root = d.screen().root

dw = root.get_geometry().width
dh = root.get_geometry().height

sw=w
sh=h
offx=0
if fullscreen:
	sw = (int)(w*(dh/h))
	sh=dh
	offx = (int)((dw-sw)/2)



def grab_bytes():
	raw = root.get_image(offx,0,sw,sh, X.ZPixmap, 0xffffffff)
	image = Image.frombytes("RGB", (sw, sh), raw.data, "raw", "BGRX")
	image = ImageOps.invert(image)
	#image.show()
	image = (image.resize((w,h))
		.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
		.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
		.convert("1", dither=dither))
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


