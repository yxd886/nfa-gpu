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

	opennf=[12768,74453,145280,250990,428439,859917,1573963,1816956,2191235,2246421,2465946,2177648,1790273,1791811,1770469]
	opennf=map(float,opennf)
	nfa=[3,6]
	nfa=map(float,nfa)
	
	plt.style.use('ggplot')#seaborn-white')

	fig,ax1 = plt.subplots()
	fig = plt.figure(1)

	runtimes = [1,2,3,4,5,6,7,8,9,10,11,12]
	colors = ['r','b','y','m','c','g','r','b','y']
	styles = ['-.', '--', ':', '-', '--', ':', '-','--']
	index = 0;

	x = np.arange(1)
	labels= ["1","10","30","60","120","240","480","600","700","750","800","850","960","1000","1500"]

	width = 0.3
	ax1.bar(x, opennf,width, label="OpenNF",  hatch="/")
	#ax1.bar(x+width,nfa,width, label="NFActor", hatch="\\")

	plt.xticks(x+0.5*width,labels)
	for tl in ax1.get_xticklabels():
		tl.set_fontsize(20)
		tl.set_fontstyle('normal')
	for tl in ax1.get_yticklabels():
		tl.set_fontsize(20)
		tl.set_fontstyle('normal')

	# Now add the legend with some customizations.
	legend = ax1.legend(loc='upper left', shadow=False)
	# Set the fontsize
	for label in legend.get_texts():
		label.set_fontsize(20)

	for label in legend.get_lines():
		label.set_linewidth(3)  # the legend line width

	plt.xlabel("size of packet batch(pkts)", fontsize=25, style='normal', color='black')
	plt.ylabel("Throughput(pps)", fontsize=25, style='normal', color='black')
	plt.savefig("WA.pdf", bbox_inches='tight', pad_inches=0)
	plt.show()

def main():

	#runtimes,received,dropped,time = read_log("temp")

	print draw() 



if __name__ == "__main__" :
	main()