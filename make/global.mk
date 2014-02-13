################################################################################
##
## Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
##							 2014			 Christoph Steup <steup@ivs.cs.ovgu.de>
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

include $(FAMOUSOROOTDIR)/config.mk

ifeq (${CXX},g++)
VERSION      := $(shell ${CXX} -dumpversion)
else 
ifeq (${VERSION},)
$(error Please set the version of your compiler as VERSION in config.mk)
endif
endif

CC		        ?= gcc
CXX		        ?= g++
AR		        ?= ar
ARFLAGS		    ?= ru
RANLIB		    ?= ranlib
LD		        ?= $(CXX)
AS		        ?= as

ENVIRONMENT   := ${PLATFORM}/${FLAVOR}/${CXX}-${VERSION}
EXTERNALSDIR  := $(FAMOUSOROOTDIR)/externals
INCDIR        := $(FAMOUSOROOTDIR)/include
SRCDIR        := $(FAMOUSOROOTDIR)/src
BUILDDIR      := $(FAMOUSOROOTDIR)/build/${ENVIRONMENT}
BUILDDIRBASE  := $(FAMOUSOROOTDIR)/build
LIBDIR        := $(FAMOUSOROOTDIR)/lib/${ENVIRONMENT}
LIBDIRBASE    := $(FAMOUSOROOTDIR)/lib
SAMPLEDIR     := $(FAMOUSOROOTDIR)/testsuite

DIRS          := ${LIBDIR} ${BUILDDIR}

LIBNAME       := famouso
LIBFAMOUSO    := $(LIBDIR)/lib$(LIBNAME).a

LDPATHS       := $(LIBDIR)
LIBS          := ${LIBNAME}
INCLUDES      += $(INCDIR)

FAMOUSO_DEBUG ?= -g -DFAMOUSO_DEBUG_DISABLE

CFLAGS        := -Os -Wall $(FAMOUSO_DEBUG) -fno-strict-aliasing ${CFLAGS}
CXXFLAGS      := ${CFLAGS} ${CXXFLAGS}
LDFLAGS       := -Wl,--gc-sections

-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/defs.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/gcc.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(FLAVOR)/tags.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(FLAVOR)/additional_defs.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(FLAVOR)/gcc.mk
include ${FAMOUSOROOTDIR}/make/externalsDefs.mk
