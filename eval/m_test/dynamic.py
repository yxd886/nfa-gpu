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
	#runtimes = []
	received = []
	counter=0
	#dropped = []
	#time = []
	with open(filename) as f:
		for line in f:
			if line.find("PROCESS_TIME") != -1:
				counter+=1
				numbers = line.split(':')
				received.append(int(numbers[1])*32)
				if counter==150:
					break
				#dropped.append(float(numbers[2]))
				#time.append(float(numbers[3]))

	return  received

def draw(a,b):
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
	timeline=np.linspace(0,7.5,150)
	labels= [50,15,15.2,15.4,20]

	width = 0.3

#ax2=ax1.twinx()
	ax1.plot(timeline, a,"r-.", label="FM->FW(60rules)->LB",  linewidth=3)
	ax1.plot(timeline, b,"b--", label="FM->FW(180rules)->LB",  linewidth=3)


#	ax2.plot(labels, throughput,"y*-.", label="Throughput(pps)",  linewidth=3)
	#plt.xticks(x+0.5*width,labels)
	legend = ax1.legend(loc='upper left', shadow=False)
	for label in legend.get_texts():
		label.set_fontsize(12)

	for label in legend.get_lines():
		label.set_linewidth(3)  # the legend line width
	for tl in ax1.get_xticklabels():
		tl.set_fontsize(10)
		tl.set_fontstyle('normal')
	for tl in ax1.get_yticklabels():
		tl.set_fontsize(10)
		tl.set_fontstyle('normal')

	# Now add the legend with some customizations.
	#legend = ax1.legend(loc='upper left', shadow=False)
	# Set the fontsize
	#for label in legend.get_texts():
	#	label.set_fontsize(12)

	#for label in legend.get_lines():
	#	label.set_linewidth(3)  # the legend line width

	plt.xlabel("Time(s)", fontsize=15, style='normal', color='black')
	plt.ylabel("Batch size(pkts)", fontsize=15, style='normal', color='black')
#	plt.ylabel("Throughput(pps)", fontsize=25, style='normal', color='black')
	plt.savefig("Wdy1.pdf", bbox_inches='tight', pad_inches=0)
	plt.show()

def main():

	#runtimes,received,dropped,time = read_log("temp")

	a=read_log("60log")
	b=read_log("rt1_log.log")
	print draw(a,b) 



if __name__ == "__main__" :
	main()
