# ESP8266 Kindle Touch Simulator
Simulate touchscreen \"presses\" remotely on the Kindle.

## Kindle
### Tested on:
Kindle Paperwhite 3

### Requirements
- [Jailbroken Kindle](https://wiki.mobileread.com/wiki/5_x_Jailbreak)
- [USBNetwork extension](https://www.mobileread.com/forums/showthread.php?t=225030)
- Static IP address for the Kindle
- Shell access (over SSH, Telnet, etc.)

### Installation
1. Copy `esptouch/` folder into root of Kindle (`/mnt/us/`)
2. Copy `esptouch.conf` to `/etc/upstart/` (run `mntroot rw` to allow writing to `/etc`)

## ESP8266
### Installation
1. Wire up buttons to the ESP8266.
2. Modify Arduino sketch with Kindle IP, WiFi network credentials, I/O configuration.
3. Upload Arduino sketch to ESP8266

## References
- [Kindle Web Remote Control](https://www.instructables.com/Kindle-Web-Remote-Control/)
- [Kindle WiFi Remote Footswitch](https://www.instructables.com/Kindle-WiFi-Remote-Footswitch/)
- [Keeping the Kindle awake](http://www.mobileread.mobi/forums/showthread.php?t=220810&page=3)
- [Running scripts on Kindle startup](https://www.mobileread.com/forums/showthread.php?t=221019)
