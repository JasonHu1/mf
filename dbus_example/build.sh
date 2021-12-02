#!/bin/bash

#compile in raberrypi 4b
if [ -f a.out ];then
    rm a.out
fi
gcc otbr_object.cpp  -lstdc++ -ldbus-1 -I/usr/include/dbus-1.0 -I/usr/lib/aarch64-linux-gnu/dbus-1.0/include