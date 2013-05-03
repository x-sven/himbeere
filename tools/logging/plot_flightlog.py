# plot logfiles (quicklook)

import tkFileDialog
from pylab import *
import numpy as np
from numpy import *

import csv

# get filename
loadfile = tkFileDialog.askopenfilename(title='Select a logfile...',
filetypes=[('Files','*.log')],defaultextension='.log',initialdir="../")

# toDo: read this file in a better way!!!
timeS = []
timeUS = []
ax = []
ay = []
az = []
gx = []
gy = []
gz = []
mx = []
my = []
mz = []
pressure = []
temp = []
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
           if len(line) == 15:
               timeS   = np.append(timeS, int(line[0]))
	       timeUS   = np.append(timeUS, int(line[1]))
               ax       = np.append(ax, float(line[2]))
               ay       = np.append(ay, float(line[3]))
               az       = np.append(az, float(line[4]))
               gx       = np.append(gx, float(line[5]))
               gy       = np.append(gy, float(line[6]))
               gz       = np.append(gz, float(line[7]))
               mx       = np.append(mx, float(line[8]))
               my       = np.append(my, float(line[9]))
               mz       = np.append(mz, float(line[10]))
               pressure = np.append(pressure, float(line[11]))
               temp     = np.append(temp, float(line[12]))
               runtime_ms= np.append(runtime_ms, float(line[13]))
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

ACC_SCALE = 1; #(32*9.81)/np.power(2,16);
GYRO_SCALE = 1; # (4000.)/np.power(2,16);
MAG_SCALE = 1;


#------------------------------------
figure(1)
subplot(3,1,1)
plot(time, ax*ACC_SCALE)
title('Acceleration')
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, ymin, ymax])
grid(True)
ylabel('ax in m/s')

subplot(3,1,2)
plot(time, ay*ACC_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('ay in m/s')

subplot(3,1,3)
plot(time, az*ACC_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('az in m/s')
xlabel('Time in s')
savefig('Acceleration.svg')

#------------------------------------
figure(2)
subplot(3,1,1)
plot(time, gx*GYRO_SCALE)
title('Rotational Rates')
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, ymin, ymax])
grid(True)
ylabel('rx in deg/s')

subplot(3,1,2)
plot(time, gy*GYRO_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('ry in deg/s')

subplot(3,1,3)
plot(time, gz*GYRO_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('rz in deg/s')
xlabel('Time in s')
savefig('Rotation.svg')

#------------------------------------
figure(3)
subplot(3,1,1)
plot(time, mx*MAG_SCALE)
title('Magnetic Field')
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, ymin, ymax])
grid(True)
ylabel('mx in mT')

subplot(3,1,2)
plot(time, my*MAG_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('my in mT')

subplot(3,1,3)
plot(time, mz*MAG_SCALE)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('mz in mT')
xlabel('Time in s')
savefig('Magnetometer.svg')

#------------------------------------
figure(4)
subplot(2,1,1)
title('Barometer')
plot(time, pressure)
#[ymin,ymax] = mm.minmax(ax, False)
axis([xmin, xmax, 1000, 1020])
grid(True)
ylabel('p in Pa')

subplot(2,1,2)
plot(time, temp)
#[ymin,ymax] = mm.minmax(ax, False)
#axis([xmin, xmax, -degmax, degmax])
grid(True)
ylabel('T in deg C')
xlabel('Time in s')
savefig('Barometer.svg')

#------------------------------------
figure(5)
title('Timing')
plot(time, runtime_ms, label='active')
hold(True)
plot(time, delta_t, label='delta_t')
legend()
grid(True)
xlabel('Time in s')
savefig('Timing.svg')


show()
exit()
