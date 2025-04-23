#!/bin/sh

echo $DEVPATH | awk 'match($0,/[0-9](-[0-9])+(\.[0-9])*\:[0-9](\.[0-9])+/) { print substr($0,RSTART,RLENGTH) }' > /sys/bus/usb/drivers/ftdi_sio/unbind

