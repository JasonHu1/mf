#!/bin/bash

#silabs ssd20x
arm-linux-gnueabihf-gcc ubus_server.c -L./lib \
-lubus -lubox -lblobmsg_json -ljson-c \
 -I./lib/inc -I./lib/inc/libubox \
-o ubusServer

arm-linux-gnueabihf-gcc ubus_client.c -L./lib \
-lubus -lubox -lblobmsg_json -ljson-c \
 -I./lib/inc -I./lib/inc/libubox \
-o ubusClient