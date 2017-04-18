#!/usr/bin/env python2.7
import os
import optparse
import sys
import subprocess
import signal
import time

import numpy as np
import matplotlib.pyplot as plt


def read_log(filename):
	runtimes = []
	received = []
	dropped = []
	time = []
	with open(filename) as f:
		for line in f:
			if line.find("[RESULT]") != -1:
				numbers = line.split(' ')
				received.append(float(numbers[1]))
				dropped.append(float(numbers[2]))
				time.append(float(numbers[3]))

	return runtimes, received, dropped, time 

def draw():

	gpu=[19.8145,19.9514,19.9198,16.0124]
	gpu=map(float,gpu)
	waiting=[13.9257,10.7317,1.9083,0.17368]
	waiting=map(float,waiting)
	cpu=[6.038,8.453,17.6,19.8]
	cpu=map(float,cpu)
	total=[20,20,20,20]
	total=map(float,total)

	plt.style.use('ggplot')#seaborn-white')

	fig,ax1 = plt.subplots()
	fig = plt.figure(1)

	runtimes = [1,2,3,4,5,6,7,8,9,10,11,12]
	colors = ['r','b','y','m','c','g','r','b','y']
	styles = ['-.', '--', ':', '-', '--', ':', '-','--']
	index = 0;

	x = np.arange(4)
	labels= ["15","25","30","60"]

	width = 0.2
	ax1.bar(x, gpu,width, label="GPU processing time",  hatch="/")
	ax1.bar(x+width,waiting,width, label="CPU waiting time", hatch="\\")
	ax1.bar(x+2*width,cpu,width, label="CPU processing time", hatch="~")
	ax1.bar(x+3*width,total,width, label="total time", hatch="-")


	plt.xticks(x+0.5*width,labels)
	for tl in ax1.get_xticklabels():
		tl.set_fontsize(10)
		tl.set_fontstyle('normal')
	for tl in ax1.get_yticklabels():
		tl.set_fontsize(10)
		tl.set_fontstyle('normal')

	# Now add the legend with some customizations.
	legend = ax1.legend(loc='upper left', shadow=False)
	# Set the fontsize
	for label in legend.get_texts():
		label.set_fontsize(8)

	for label in legend.get_lines():
		label.set_linewidth(3)  # the legend line width

	plt.xlabel("size of packet batch(kpkts)", fontsize=18, style='normal', color='black')
	plt.ylabel("Time(s)", fontsize=18, style='normal', color='black')
	plt.savefig("WM.pdf", bbox_inches='tight', pad_inches=0)
	plt.show()

def main():

	#runtimes,received,dropped,time = read_log("temp")

	print draw() 



if __name__ == "__main__" :
	main()
