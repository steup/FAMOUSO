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

include ../../make/Makefile
INC=../include
FILE=../include/famouso.i
GEN=generated

ifeq ($(PLATFORM),windows)
 LDFLAGS+=-Wl,--add-stdcall-alias -o $(LIBNAME).dll
else
 CXXFLAGS+=-fpic
 LDFLAGS+=-o lib$(LIBNAME).so
endif


all:
	mkdir -p $(GEN)
	swig -csharp -c++ -outdir $(GEN) -o famouso_wrap.cxx -namespace famouso -I../include/ $(FILE)
	$(CXX) $(CXXFLAGS) -c ../client/famouso.cc -o $(GEN)/famouso.o -I../../include -I$(INC)
	$(CXX) $(CXXFLAGS) -c famouso_wrap.cxx -o $(GEN)/famouso_wrap.o -I../../include -I$(INC)
	$(CXX) -shared $(GEN)/famouso.o $(GEN)/famouso_wrap.o $(CXXFLAGS) $(LDFLAGS)
	mcs -out:Subscriber.exe $(GEN)/*.cs Subscriber.cs
	mcs -out:Publisher.exe $(GEN)/*.cs Publisher.cs

clean:
	rm -fr $(GEN) famouso_wrap.* libfamouso.so famouso.dll *.exe

