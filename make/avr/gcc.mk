################################################################################
##
## Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
## All rights reserved.
##
##    Redistribution and use in source and binary forms, with or without
##    modification, are permitted provided that the following conditions
##    are met:
##
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in
##      the documentation and/or other materials provided with the
##      distribution.
##
##    * Neither the name of the copyright holders nor the names of
##      contributors may be used to endorse or promote products derived
##      from this software without specific prior written permission.
##
##
##    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
##    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
##    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
##    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
##    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
##    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
##    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
##    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##
## $Id$
##
################################################################################

CC      = avr-gcc
CXX     = avr-g++
AR      = avr-ar
ARFLAGS = ru
RANLIB  = avr-ranlib
LD      = avr-ld
AS      = avr-as
OBJCOPY	= avr-objcopy
AVRDUDE = avrdude

MCU=at90can128
ADDITIONAL_CFLAGS += -DAVR -mmcu=$(MCU) -fno-threadsafe-statics

################################################################################
#AVRDUDE_PORT = /dev/parport0  # programmer connected to serial device
#AVRDUDE_PORT = /dev/pcan1     # programmer connected to CAN
#AVRDUDE_PORT = /dev/ttyS0   # programmer connected to serial device
AVRDUDE_PORT = /dev/ttyUSB0   # programmer connected to serial device

AVRDUDE_PROGRAMMER = avr911
#AVRDUDE_PROGRAMMER = pcan
#AVRDUDE_PROGRAMMER = stk200

AVRDUDE_FLAGS        = -v -P $(AVRDUDE_PORT) -u -c $(AVRDUDE_PROGRAMMER) -p $(MCU)
################################################################################

%.elf:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)

%.hex:%.elf
	@$(RULEECHO) ; \
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.program:%.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U f:w:$<:a

