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
-include ../../make/Makefile

filtered=$(filter-out %Makefile,$(shell find . -maxdepth 1 -name "*avr*"))

.phony: all clean distclean

all: build

avr-halib:
	@echo Checking out latest AVR-halib
	@svn export https://svn-eos.cs.uni-magdeburg.de/repos/Projects/AVR/halib/trunk avr-halib --force --ignore-externals -q

build-avr-halib:
	@echo Building avr-halib
	@make -C avr-halib ${MCU} portmapgen
	@cp avr-halib/build/libavr-halib-${MCU}.a ${LIBDIR}

avr-maclib:
	@echo Checking out latest AVR-maclib
	@svn export https://svn-eos.cs.uni-magdeburg.de/repos/stud/mafoerst/Code/maclib avr-maclib --force --ignore-externals -q

build-avr-maclib:
	@echo Building avr-maclib
	@make -C avr-maclib HALIBDIR=../avr-halib portmap ${MCU}
	@cp avr-maclib/build/libavr-maclib-${MCU}.a ${LIBDIR}

build: avr-halib build-avr-halib avr-maclib build-avr-maclib

clean:
	make -C avr-halib clean
	make -C avr-maclib clean



distclean:
	@rm -rf $(filtered)
