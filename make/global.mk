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

MACHINETYPE = $(shell $(CXX) -dumpmachine)
GCCVERSION  = gcc-$(shell $(CXX) -dumpversion)

EXTERNALSDIR= $(FAMOUSOROOTDIR)/externals

MODDIRBASE  = $(FAMOUSOROOTDIR)/module
MODULEDIR   = $(MODDIRBASE)/$(MACHINETYPE)/$(GCCVERSION)

INCDIR      = $(FAMOUSOROOTDIR)/include

SRCDIR      = $(FAMOUSOROOTDIR)/src

DEPDIRBASE  = $(FAMOUSOROOTDIR)/depend
DEPENDDIR   = $(DEPDIRBASE)/$(MACHINETYPE)/$(GCCVERSION)

SAMPLEDIR   = $(FAMOUSOROOTDIR)/testsuite

LIBNAME     = famouso
LIBBASE     = $(FAMOUSOROOTDIR)/lib
LIBDIR      = $(LIBBASE)/$(MACHINETYPE)/$(GCCVERSION)
LIBFAMOUSO  = $(LIBDIR)/lib$(LIBNAME).a

LIB         = -L$(LIBDIR) -lfamouso
INCLUDE     = -I$(INCDIR) -I$(SRCDIR)

FAMOUSO_DEBUG ?= -g -DFAMOUSO_DEBUG_DISABLE

# external header dependencies
# this variable definition leads to downloading and checking out
# the needed externals. In the include dir resides all header only
# dependencies
EXTERNALS    = $(EXTERNALSDIR)/include
# define Boost also as external
EXTERNALS   += $(EXTERNALSDIR)/Boost

# prefix until now defined externals with -I to allow using it as
# include path for compiler runs
ADDITIONAL_CFLAGS := $(patsubst %,-I%,$(EXTERNALS))

ifneq ($(CONFIG),avr)
LIBBOOST             = $(LIBDIR)/libboost_system.a $(LIBDIR)/libboost_thread$(THREADTAG).a $(LIBDIR)/libboost_program_options.a
EXTERNALS           += $(LIBBOOST)
ADDITIONAL_LIBS     += $(LIBBOOST)
endif


OPTLEVEL	= -Os

ADDITIONAL_CFLAGS   += -fno-strict-aliasing

CCFLAGS     = -Wall $(FAMOUSO_DEBUG) -I$(INCDIR) $(ADDITIONAL_CFLAGS)
CCOPTIONS   = $(OPTLEVEL)

CXXOPTIONS  = $(CCOPTIONS)
CXXFLAGS    = -Wall $(FAMOUSO_DEBUG) -I$(INCDIR) $(ADDITIONAL_CFLAGS)

LDFLAGS     = $(LIBFAMOUSO) $(ADDITIONAL_LIBS) $(LIBFAMOUSO)
