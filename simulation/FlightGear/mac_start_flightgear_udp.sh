#!/bin/sh

FG_ROOT=/Users/sven/Programme/FlightGear.app/Contents/Resources/data
FG_SCENERY=$FG_ROOT/Scenery

/Users/sven/Programme/FlightGear.app/Contents/MacOS/fgfs \
    --prop:/sim/model/path=/Models/Aircraft/Tricopter/Tri-o-copter.ac \
    --control=joystick \
    --disable-random-objects \
    --prop:/sim/rendering/random-vegetation=false \
    --disable-ai-models \
    --disable-ai-traffic \
    --disable-real-weather-fetch \
    --disable-sound \
    --disable-intro-music \
    --disable-terrasync \
    --airport=EDVE \
    --fdm=null \
    --bpp=32 \
    --atlas=socket,out,5,localhost,5505,udp \
    --native-fdm=socket,in,25,,5507,udp

#    --enable-terrasync \


#"C:\Program Files\FlightGear\bin\Win32\fgfs.exe" ^
#  --fg-root="C:\Program Files\FlightGear\data" ^
#  --fg-scenery="C:\Program Files\FlightGear\data\Scenery;C:\Program Files\FlightGear\scenery;C:\Program Files\FlightGear\terrasync" ^
#  --control=joystick ^
#  --disable-random-objects ^
#  --prop:/sim/rendering/random-vegetation=false ^
#  --prop:/sim/model/path=Models/tri.ac ^
#  --disable-ai-models ^
#  --disable-ai-traffic ^
#  --disable-real-weather-fetch ^
#  --disable-sound ^
#  --disable-intro-music ^
#  --fdm=null ^
#  --bpp=32 ^
#  --atlas=socket,out,5,localhost,5505,udp ^
#  --native-fdm=socket,in,25,,5507,udp
