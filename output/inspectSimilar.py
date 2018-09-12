#!/usr/bin/python
#import matplotlib
#matplotlib.use('Agg')
import matplotlib
#matplotlib.use('Agg')
import matplotlib.pyplot as plt
import sys
import gzip
import math
import glob
import os


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

def errorImg(img1, img2):
	error = 0.0
	for i in range(len(img1)):
		for j in range(len(img1[i])):
			error += (img1[i][j] - img2[i][j])**2  

	return error



def main():

	files = glob.glob('img*.raw.gz')

	idx1 = 0        
	while idx1<len(files):
		
		file1 = files[idx1]

		maxError = 3000000000.0
		currentError = 0.0
		idx2 = idx1+1
		delList = []
		while currentError < maxError:
			file2 = files[idx2]

			buff1 = loadImage(file1,_W*_H*_D)
			buff2 = loadImage(file2,_W*_H*_D)

	                if buff1 != None and buff2 != None:
	                        img1 = reshapeArray(buff1, _W, _H, _D)
				img2 = reshapeArray(buff2, _W, _H, _D)

				currentError = errorImg(img1, img2)

				print(file1 + ' - ' + file2 + ' = %f'%currentError)
	
				if currentError < maxError:
					delList += [file2]

			idx2 += 1

		idx1 = idx2

		for file in delList:
			os.remove(file)

		print(file1 + ' - ' + file2)
		print(delList)
		#plt.subplot(1,2,1)
		#plt.imshow(img1)
		#plt.subplot(1,2,2)
		#plt.imshow(img2)
		#plt.show()		

if __name__=='__main__':
	main()
