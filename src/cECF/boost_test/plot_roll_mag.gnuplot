#!/usr/bin/gnuplot -persist

plot "history_roll_mag.txt" u 1:2 title "Pitch",\
     "history_roll_mag.txt" u 1:3 title "Roll",\
     "history_roll_mag.txt" u 1:4 title "Yaw"
set mxtics 5.000000
set mytics 5.000000
     
#plot "history_pitch.txt" u 1:2, "history_pitch.txt" u 1:3, "history_pitch.txt" u 1:4

