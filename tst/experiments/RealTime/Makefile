################################################################################
##
## Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

CONFIG=linux/xenomai
#CONFIG=avr
#ADD_FLAGS=-Wfatal-errors


include ../../../make/Makefile
ADDITIONAL_CFLAGS+= -I../../../Bindings/include

SOURCES=$(wildcard *.cc)
BINS=$(SOURCES:.cc=)
AVR_SOURCES=$(wildcard AVR_*.cc)
AVR_BINS=$(AVR_SOURCES:.cc=)

.PHONY: all clean

ifneq ($(OPT),)
CXXFLAGS += -DNDEBUG -DFAMOUSO_DEBUG_DISABLE -DLOGGING_DISABLE
endif

ifneq ($(ADD_FLAGS),)
CXXFLAGS += $(ADD_FLAGS)
endif

ifeq ($(CONFIG),avr)
CXXFLAGS += -Os
LDFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
endif

eval: Nodes RT_Master

all: $(BINS)

avr: $(AVR_BINS)
	avr-size $(AVR_BINS)


Nodes: Node1 Node2 Node3 Node4



clean:
	rm -f $(BINS) *.d *.hex *.elf

-include *-$(MACHINETYPE)-$(GCCVERSION).d
