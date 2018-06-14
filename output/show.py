#!/usr/bin/python
import matplotlib.pyplot as plt
import sys
import gzip
import math

_W = 640
_H = 512
_D = 2

def reshapeArray(buffer, w, h, d):
	img = [[0 for i in range(w)] for j in range(h)]
	for i in range(len(buffer)/d):
		#print('i = %d, _=%d, _=%d'%(i, int(i/512), i%640))
		tmp = 0
		for k in range(d):
			tmp = tmp + (256**k)*ord(buffer[d*i+k])
		img[int(i/w)][i%w] = float(tmp)

	return img
	
def loadImage(fileName, size):
	buffer = None
	try :
		fid = gzip.open(fileName,'rb')
		buffer = fid.read(size)
		fid.close()

	finally:
		
		return buffer


n1 = int(sys.argv[1])
n2 = int(sys.argv[2])

qtd = n2-n1+1

NRows = int(math.sqrt(qtd) + 0.5)
NCols = int(float(qtd)/NRows -0.001) + 1

row, col = 0,0

print(NRows, NCols)

for n in range(n1, n2+1):
	print(n)

	buff = loadImage('img%d.raw.gz'%n, _W*_H*_D)

	if buff != None:
		img = reshapeArray(buff, _W, _H, _D)

		ax = plt.axes([0.01 + float(row)/NRows, 0.01 + float(col)/NCols, 0.97/NRows, 0.97/NCols])
		ax.axis('off')
		imgplot = plt.imshow(img)

		row += 1
		if row>=NRows:
			row = 0
			col += 1

plt.show()
