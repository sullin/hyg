# Xiaomi Mijia measurement reading utility
This utility reads the Xiaomi Mijia bluetooth thermometer/hygrometer (LYWSDCGQ/01ZM) readings. The result is printed to standard output to be graphed with munin. It is standalone and does not depend on any runtime libraries (although header files in libbluetooth-dev are needed during compiling).

## Usage
Find the address of the device:
```
$ sudo hcitool lescan
LE Scan ...
11:22:33:44:55:66 MJ_HT_V1
```

Read the sensor:
```
$ ./hyg 11:22:33:44:55:66
temp.value=24.9
hyg.value=26.6
```

## Cross compiling
Cross compiling to run on Synology (ARM)
```
sudo apt-get install gcc-arm-linux-gnueabi
sudo apt-get install libc6-dev-armel-cross
CC=arm-linux-gnueabi-gcc make
```
