#!/usr/bin/python

import glob
import os
import sys
import glob

files = glob.glob('img*.raw.gz')

count = 1
for file in files:
	os.rename(file, 'img%d.raw.gz'%count)
	print(file, 'img%d.raw.gz'%count)
	count += 1
	

