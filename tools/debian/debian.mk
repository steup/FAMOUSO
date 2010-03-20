################################################################################
##
## Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
##               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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

include Makefile

HEADERS=$(shell cd include && find . -name "*.h")
BIN=$(DESTDIR)/usr/bin
LIB=$(DESTDIR)/usr/lib/famouso
INC=$(DESTDIR)/usr/include/famouso

debian-all: debian-build

debian-build: all
	$(MAKE) -C ECHs ech-awds
	$(MAKE) -C tst/experiments/QoS Publisher
	$(MAKE) -C tst/experiments/QoS Subscriber

debian-install: debian-build
	@install -d $(DESTDIR)/usr/bin
	@echo "installing binary to $(BIN)"
	@install -T ECHs/ech-awds $(BIN)/famouso-ech
	@install -T tst/experiments/QoS/Publisher $(BIN)/famouso-pub
	@install -T tst/experiments/QoS/Subscriber $(BIN)/famouso-sub

	@echo "installing libs to $(LIB)"
	@install -d $(LIB)
	@install -t $(LIB) $(LIBDIR)/*.a

	@echo "installing header to $(INC)"
	@for h in $(subst ./,,$(HEADERS)); do \
		install -D include/$$h $(INC)/$$h; \
	done

debian-clean: properclean
	$(MAKE) -C ECHs clean
	$(MAKE) -C tst/experiments/QoS clean

.PHONY: debian-all debian-build debian-install debian-clean
