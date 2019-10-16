# Xiaomi Mijia measurement reading utility
This utility reads the Xiaomi Mijia bluetooth sensors. The result is printed to standard output to be graphed with munin. It is a simple standalone utility and does not depend on any runtime libraries (although header files in libbluetooth-dev are needed during compiling). It keeps the bluetooth traffic to the minimum, handle lookups are hardcoded.

## Supported devices:
* LYWSDCGQ/01ZM (thermometer/hygrometer)
* LYWSD02MMC (clock/thermometer/hygrometer)

## Features:
* Reading temperature and humidity
* Setting clock (for LYWSD02MMC)

## Usage
Find the address of the device:
```
$ sudo hcitool lescan
LE Scan ...
11:22:33:44:55:66 MJ\_HT\_V1
11:22:33:44:55:67 LYWSD02
```

Read the sensors:
```
$ ./hyg 11:22:33:44:55:66
temp.value 24.9
hum.value 26.6

$ ./hyg -c -t 02\_0042 11:22:33:44:55:67
temp.value 26.5
hum.value 48.0
```

## Cross compiling
Cross compiling to run on Synology (ARM)
```
sudo apt-get install gcc-arm-linux-gnueabi
sudo apt-get install libc6-dev-armel-cross
CC=arm-linux-gnueabi-gcc make
```
