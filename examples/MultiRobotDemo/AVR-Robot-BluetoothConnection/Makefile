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

C	= .cc
O	= .o

NAME	= DemoHannover

SRC	= $(basename $(wildcard *$(C)))
TARGET_C  = $(wildcard *$(C))
TARGET_O  = $(addsuffix $(O), $(SRC))

CTRL    = at90can128
INCLUDE = ../include
STK	= avrisp

CC      = avr-g++
OC      = avr-objcopy
LDFLAGS = -j .text -j .data

CFLAGS  = -g -O2 -Wall

AVRDUDE = avrdude

AVRDUDE_FLAGS =	-P /dev/parport0
AVRDUDE_FLAGS += -c $(STK)
AVRDUDE_FLAGS += -p $(CTRL)
AVRDUDE_FLAGS += -U f:w:$(NAME).hex:a

DEPENDFILE = .depend

all:	$(TARGET_O)
	$(CC) -o $(NAME).elf $(CFLAGS) -I $(INCLUDE) -mmcu=$(CTRL) $(TARGET_O)

dep:
	@echo "---- analyse dependencies -----" 
	#rm $(DEPENDFILE)
	$(CC) -MM $(TARGET_C) > $(DEPENDFILE)




%.o:%$(C)
	$(CC) -c $(CFLAGS) -I $(INCLUDE) -mmcu=$(CTRL)  $< -o $@


hex:	all
	@echo "generate properly output format"
	$(OC) $(LDFLAGS) -O ihex $(NAME).elf $(NAME).hex

program: hex
	@echo "Programing the device."
	$(AVRDUDE) $(AVRDUDE_FLAGS)

clean:
	@echo "Cleaning"
	@rm -rf $(TARGET_H) $(TARGET_O) $(NAME).elf *~

-include $(DEPENDFILE)
