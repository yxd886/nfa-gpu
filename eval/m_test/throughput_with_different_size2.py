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
	gpunfv60=[145280,250990,428439,859917,1573963,1816956,2191235,2265946,2177648,2172848,2265966,2265966,2265966,2265946]
	gpunfv60=map(float,gpunfv60)
	nfa=[1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568,1007568]
	nfa=map(float,nfa)
	gpunfv180=[80320,141303,261030,501501,809208,1126110,1207630,1300852,1408756,1571664,1633892,1984527,1974534,1968524]
	gpunfv180=map(float,gpunfv180)
	nfa180=[604857,604857,604857,604857,604857,604857,604857,604857,604857,604857,604857,604857,604857,604857]
	nfa180=map(float,nfa180)	
	plt.style.use('ggplot')#seaborn-white')

	fig,ax1 = plt.subplots()
	fig = plt.figure(1)

	runtimes = [1,2,3,4,5,6,7,8,9,10,11,12]
	colors = ['r','b','y','m','c','g','r','b','y']
	styles = ['-.', '--', ':', '-', '--', ':', '-','--']
	index = 0;

	#x = np.arange(14)
	labels= [1,2,4,8,16,19,22,25,27,30,32,35,43,48]

	width = 0.3
	ax1.plot(labels, gpunfv60,"r-.", label="GPUNFV(60rules)",  linewidth=3)
	ax1.plot(labels,nfa,"b--", label="NFActor(60rules)", linewidth=3)
	ax1.plot(labels, gpunfv180,"y*-.", label="GPUNFV(180rules)",  linewidth=3)
	ax1.plot(labels,nfa180,"y*:",label="NFActor(180rules)", linewidth=3)
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
		label.set_fontsize(17)

	for label in legend.get_lines():
		label.set_linewidth(3)  # the legend line width

	plt.xlabel("size of packet batch(kpkts)", fontsize=25, style='normal', color='black')
	plt.ylabel("Throughput(pps)", fontsize=25, style='normal', color='black')
	plt.savefig("WT.pdf", bbox_inches='tight', pad_inches=0)
	plt.show()

def main():

	#runtimes,received,dropped,time = read_log("temp")

	print draw() 



if __name__ == "__main__" :
	main()
