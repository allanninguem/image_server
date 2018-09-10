#!/usr/bin/python

import glob
import os
import sys

n1 = eval(sys.argv[1])
n2 = eval(sys.argv[2])

for i in range(n1,n2+1):
	try:
		os.remove('img%d.raw.gz'%i)
	except:
		pass

