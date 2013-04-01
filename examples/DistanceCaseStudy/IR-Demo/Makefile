################################################################################
##
## Copyright (c) 2008-2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
##    $Id$
##
################################################################################
ARToolKit=../../../../../ARToolKit
include ../../../make/Makefile
CXXFLAGS += -I../../../Bindings/include \
          	-I$(ARToolKit)/include \
			-I/usr/include/glib-2.0 \
			-I/usr/lib/glib-2.0/include \
			-I/usr/include/libxml2 \
			-I/usr/X11R6/include \
			-I/usr/include/gstreamer-0.10

ADDITIONAL_LIBS+=  	-L$(ARToolKit)/lib \
					-lARgsub \
					-lARvideo \
					-lAR \
					-lglut \
					-lGLU \
					-lGL \
					-lXi \
					-lX11 \
					-lgstreamer-0.10 \
					-lgobject-2.0 \
					-lgmodule-2.0 \
					-lgthread-2.0 \
					-lrt \
					-lxml2 \
					-lglib-2.0
		 			#-L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR)

OBJS = 	demo.o \
		Sensor.o \
		arObject.o \
		arSensor.o \
		glDiagram.o \
		text3d.o

filtered=$(filter-out %.cc %.h %.py %.impl %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all clean

all: demo

clean:
	@rm -rf $(filtered)

# rule for creating the depend file and
# for linking the application and give it
# the correct name dependend of the platform
demo:$(OBJS)
	@$(RULEECHO) ; \
	$(CXX) $^ -o $@.foobar $(CXXFLAGS) $(CXXOPTIONS) $(LDFLAGS) ; \
	mv $@.foobar $(basename $@)$(SUFFIX)
