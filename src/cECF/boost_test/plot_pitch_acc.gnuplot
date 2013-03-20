#!/usr/bin/gnuplot -persist

plot "history_pitch_acc.txt" u 1:2 title "Pitch",\
     "history_pitch_acc.txt" u 1:3 title "Roll",\
     "history_pitch_acc.txt" u 1:4 title "Yaw"
set mxtics 5.000000
set mytics 5.000000
     
