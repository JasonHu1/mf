#!/bin/bash

if [ "$1" == "clean" ];then
    make clean
elif [ "$1" == "gcc" ];then
    echo "compile for self"
    make NO_READLINE=1
else
    echo "compile for cross plaform"
    make NO_READLINE=1 COMPILE_PREX=aarch64-none-linux-gnu-
fi


#header dependent
#app_driver.h
#->#app_schedue.h
#-->#app_device.h
#--->#app_defs_types.h
