##
## This file is part of the avr-gcc-examples project.
##
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2013 Ivar Holmqvist <ivarholmqvist@gmail.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

PROGRAM = nexa

# User overrideables
MCU           ?= atmega328p
F_CPU         ?= 16000000L
# Default config from arduino-core package
AVRDUDE_CONF  ?= /usr/share/arduino/hardware/tools/avrdude.conf
PROGRAMMER_ID ?= arduino
SERIAL_DEVICE ?= /dev/ttyUSB0
BAUDE_RATE    ?= 57600

CC = avr-gcc
CPP = avr-g++
OBJCOPY = avr-objcopy
CFLAGS += -Wall -g -Os  -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS +=  -Wl,--gc-sections
CPPFLAGS = -I$(ARDUINO_SRC)

ARDUINO_HW      ?= /usr/share/arduino/hardware/
ARDUINO_SRC     ?= $(ARDUINO_HW)/arduino/cores/arduino/
ARDUINO_VARIANT ?= $(ARDUINO_HW)/arduino/variants/eightanaloginputs/
CPPFLAGS += -I$(ARDUINO_VARIANT)
CPPFLAGS += -Wall -fno-exceptions -ffunction-sections -fdata-sections -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=101

OBJS += nexa_self_learning.o serial.o HardwareSerial.o Print.o WString.o main.o wiring.o

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

help:
	@echo "Available targets:"
	@echo
	@echo " make all ($(PROGRAM).hex)"
	@echo " make $(PROGRAM).hex"
	@echo " make $(PROGRAM).elf"
	@echo " make flash"
	@echo " make clean"
	@echo
	@echo "Default options:"
	@echo
	@echo " MCU=$(MCU)"
	@echo " F_CPU=$(F_CPU)"
	@echo " AVRDUDE_CONF=$(AVRDUDE_CONF)"
	@echo " PROGRAMMER_ID=$(PROGRAMMER_ID)"
	@echo " SERIAL_DEVICE=$(SERIAL_DEVICE)"
	@echo " BAUDE_RATE=$(BAUDE_RATE)"
	@echo

all: $(PROGRAM).hex

$(PROGRAM).elf: $(OBJS)
	@printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PROGRAM).hex: $(PROGRAM).elf
	@printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O ihex $< $@

%.o: %.c
	@printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CPP) $(CFLAGS) -o $@ -c $<

%.o: $(ARDUINO_SRC)/%.c
	@printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CPP) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: $(ARDUINO_SRC)/%.cpp
	@printf "  C++      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o:%.cpp
	@printf "  C++      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CPP) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

flash: $(PROGRAM).hex
	@printf "  FLASH   $(PROGRAM).hex\n"
	$(Q)avrdude -C$(AVRDUDE_CONF) -v -v -p$(MCU) -c$(PROGRAMMER_ID) -P$(SERIAL_DEVICE) -b$(BAUDE_RATE) -D -Uflash:w:$(PROGRAM).hex

clean:
	@printf "  CLEAN   $(subst $(shell pwd)/,,$(OBJS))\n"
	$(Q)rm -f $(OBJS)
	@printf "  CLEAN   $(PROGRAM).elf\n"
	$(Q)rm -f *.elf
	@printf "  CLEAN   $(PROGRAM).hex\n"
	$(Q)rm -f *.hex

