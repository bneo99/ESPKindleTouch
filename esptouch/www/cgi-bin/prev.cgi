#!/bin/sh
cat /mnt/us/esptouch/prev.event > /dev/input/event1                                                    
/usr/bin/powerd_test -i 
