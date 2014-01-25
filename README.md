nexa
====

Arduino NEXA control.

Simple control of a 433Mhz transmitter (TX433N) implementing the NEXA self learning protocol, currently just a playground for me but hopefully
it will be something useful soon.

Compile
=======

Install prequisits:
```bash
sudo apt-get install gcc-avr avr-libc avrdude arduino arduino-mk
```
*Note*, there seems to be an issue with arduino-mk on ubuntu 13.04, just harmless vid/pid warnings
for the uno board when compiling, but upgrading to ubuntu 13.10 (arduino-mk version 0.12.0-1) solves the problem.

```bash
make
make upload
```

Install
=======

Nothing yet.


Permission
==========
There a number of ways to get access to /dev/ttyUSBx here's a simple one:
```bash
sudo adduser your_user_name dialout
newgrp dialout
```
