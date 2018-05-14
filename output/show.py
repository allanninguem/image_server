#!/usr/bin/python
import matplotlib.pyplot as plt
import sys
import gzip

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
	fid = gzip.open(fileName,'rb')
	buffer = fid.read(size)
	fid.close()

	return buffer


buff = loadImage(sys.argv[1], _W*_H*_D)
img = reshapeArray(buff, _W, _H, _D)

imgplot = plt.imshow(img)
plt.show()

