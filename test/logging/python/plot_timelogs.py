# plot logfiles (quicklook)

import tkFileDialog
from pylab import *
import numpy as np
from numpy import *

import csv
import sys

# get filename

if len(sys.argv) == 1: 
	print "\nPlease provide the sourcefile with path as argument!\n"; exit();
else:
	loadfile = sys.argv[1];

# toDo: read this file in a better way!!!
timeS = []
timeUS = []
runtime_ms = []

#read whole file; ignore first line
#ifile = open(loadfile, 'r')

ifile = file(loadfile)

rownum = 0
for row in ifile:
    try:
        if rownum == 0:
            header = row;
        else:
           line = row.split("\t")
           if len(line) == 3:
               timeS   = np.append(timeS, int(line[0]))
	       timeUS   = np.append(timeUS, int(line[1]))
               runtime_ms= np.append(runtime_ms, float(line[2]))
               # last string is '\n'
        rownum += 1
    except ValueError:
        pass
ifile.close()


# let time (x-axis) start at 0
time = []
time = ((timeS )  + (timeUS /1e+6)) - ((timeS[0] )  + (timeUS[0] /1e+6))

delta_t = []
delta_t = np.append(delta_t, 0)
for ii in range(1, len(time)):
    delta_t = np.append(delta_t, (time[ii] - time[ii-1])*1000)

# set x-axis scaling
xmin = time[0]
xmax = time[-1]

#------------------------------------
figure(1)
#subplot(3,1,1)
plot(time, runtime_ms)
title('Time required to write a line of 64 bytes')
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, ymin, ymax])
grid(True)
ylabel('Writing Time in ms')
xlabel('Time in s')
savefig('Timing.svg')


show()
exit()
