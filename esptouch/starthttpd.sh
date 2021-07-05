#!/bin/sh
iptables -A INPUT -i wlan0 -p tcp --dport www -j ACCEPT;
/mnt/us/usbnet/bin/busybox httpd -h /mnt/us/esptouch/www -c /mnt/us/esptouch/httpd.conf
