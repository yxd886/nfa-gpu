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
	batchsize=[1000,25000,25320,25640,25640]
	batchsize=map(float,batchsize)

	throughput=[50498,2108520,2186544,2185654,2185618]
	throughput=map(float,throughput)

	plt.style.use('ggplot')#seaborn-white')

	fig,ax1 = plt.subplots()
	fig = plt.figure(1)

	runtimes = [1,2,3,4,5,6,7,8,9,10,11,12]
	colors = ['r','b','y','m','c','g','r','b','y']
	styles = ['-.', '--', ':', '-', '--', ':', '-','--']
	index = 0;

	#x = np.arange(14)
	labels= [0,15,15.2,15.4,20]

	width = 0.3

#ax2=ax1.twinx()
	ax1.plot(labels, batchsize,"r-.", label="Batch Size(pkts)",  linewidth=3)

#	ax2.plot(labels, throughput,"y*-.", label="Throughput(pps)",  linewidth=3)
	#plt.xticks(x+0.5*width,labels)
	for tl in ax1.get_xticklabels():
		tl.set_fontsize(10)
		tl.set_fontstyle('normal')
	for tl in ax1.get_yticklabels():
		tl.set_fontsize(20)
		tl.set_fontstyle('normal')

	# Now add the legend with some customizations.
	legend = ax1.legend(loc='upper left', shadow=False)
	# Set the fontsize
	for label in legend.get_texts():
		label.set_fontsize(12)

	for label in legend.get_lines():
		label.set_linewidth(3)  # the legend line width

	plt.xlabel("Time(s)", fontsize=25, style='normal', color='black')
	plt.ylabel("Batchsize(pkts)", fontsize=25, style='normal', color='black')
#	plt.ylabel("Throughput(pps)", fontsize=25, style='normal', color='black')
	plt.savefig("Wdy1.pdf", bbox_inches='tight', pad_inches=0)
	plt.show()

def main():

	#runtimes,received,dropped,time = read_log("temp")

	print draw() 



if __name__ == "__main__" :
	main()