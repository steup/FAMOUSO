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

MACHINETYPE = $(shell $(CXX) -dumpmachine)
GCCVERSION  = gcc-$(shell $(CXX) -dumpversion)

BINDIRBASE  = $(INSTALLDIR)/bin
BINDIR      = $(BINDIRBASE)/$(MACHINETYPE)/$(GCCVERSION)

MODDIRBASE  = $(INSTALLDIR)/module
MODULEDIR   = $(MODDIRBASE)/$(MACHINETYPE)/$(GCCVERSION)

INCDIR      = $(INSTALLDIR)/include

SRCDIR      = $(INSTALLDIR)/src

DEPDIRBASE  = $(INSTALLDIR)/depend
DEPENDDIR   = $(DEPDIRBASE)/$(MACHINETYPE)/$(GCCVERSION)

TOOLSDIR    = $(INSTALLDIR)/tools

SAMPLEDIR   = $(INSTALLDIR)/testsuite

LIBNAME     = famouso
LIBBASE     = $(INSTALLDIR)/lib
LIBDIR      = $(LIBBASE)/$(MACHINETYPE)/$(GCCVERSION)
LIBFAMOUSO  = $(LIBDIR)/lib$(LIBNAME).a

LIB         = -L$(LIBDIR) -lfamouso
INCLUDE     = -I$(INCDIR) -I$(SRCDIR)

FAMOUSO_DEBUG ?= -g -DNDEBUG

#Additional external libs

ifeq ($(CONFIG),avr)
# no additional builds defined here, see make/avr/def.mk for the definitions
ADDITIONAL_BUILDS    =
else
LIBBOOST             = $(LIBDIR)/libboost_system.a $(LIBDIR)/libboost_thread$(THREADTAG).a $(LIBDIR)/libboost_program_options.a
ADDITIONAL_BUILDS   += $(LIBBOOST)
endif

ADDITIONAL_LIBS     += $(ADDITIONAL_BUILDS)

ADDITIONAL_CFLAGS   += -fno-strict-aliasing

CCFLAGS     = -Wall $(FAMOUSO_DEBUG) -I$(INCDIR) $(ADDITIONAL_CFLAGS)
CCOPTION    = -O3

CXXOPTION   = $(CCOPTION)
CXXFLAGS    = -Wall $(FAMOUSO_DEBUG) -I$(INCDIR) $(ADDITIONAL_CFLAGS)

LDFLAGS     = $(LIBFAMOUSO) $(ADDITIONAL_LIBS)
