#!/usr/bin/gnuplot -persist

plot "history_yaw_mag.txt" u 1:2 title "Roll",\
       "history_yaw_mag.txt" u 1:3 title "Pitch",\
       "history_yaw_mag.txt" u 1:4 title "Yaw"
set mxtics 5.000000
set mytics 5.000000
set title "Test with mag"
