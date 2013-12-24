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
MCU   ?= atmega328p
F_CPU ?= 16000000L
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS += -Wall -g -Os  -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS +=

OBJS += nexa_self_learning.o

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

help:
	@echo "Available targets:"
	@echo
	@echo " make all"
	@echo " make $(PROGRAM).hex"
	@echo " make $(PROGRAM).elf"
	@echo " make flash"
	@echo " make clean"
	@echo
	@echo "Default options:"
	@echo
	@echo " MCU=$(MCU)"
	@echo " F_CPU=$(F_CPU)"

all: $(PROGRAM).hex

$(PROGRAM).elf: $(OBJS)
	@printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PROGRAM).hex: $(PROGRAM).elf
	@printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O ihex $< $@

%.o: %.c
	@printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

flash: $(PROGRAM).hex
	@printf "  FLASH   $(PROGRAM).hex\n"
	$(Q)avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:$(PROGRAM).hex

clean:
	@printf "  CLEAN   $(subst $(shell pwd)/,,$(OBJS))\n"
	$(Q)rm -f $(OBJS)
	@printf "  CLEAN   $(PROGRAM).elf\n"
	$(Q)rm -f *.elf
	@printf "  CLEAN   $(PROGRAM).hex\n"
	$(Q)rm -f *.hex

