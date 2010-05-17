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

# If FAMOUSO_INSTALLDIR isn't defined, the FAMOUSO root directory
# is automatically determind
ifeq ($(FAMOUSO_INSTALLDIR),)
# get the path of this makefile
this_makefile_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
# erase the trailing /make/ in order to get the correct root of FAMOUSO
ROOTDIR := $(patsubst %/make/,%,$(this_makefile_dir))
else
ROOTDIR := $(dir $(abspath $(FAMOUSO_INSTALLDIR)/Makefile))
endif

FAMOUSOROOTDIR ?= $(ROOTDIR)
export FAMOUSOROOTDIR

ifeq ($(wildcard $(FAMOUSOROOTDIR)/make/config.mk),)
# FAMOUSOROOTDIR is defined wrong
$(info )
$(info The FAMOUSO_INSTALLDIR you gave to FAMOUSO's make-system is wrong.              )
$(info The make-system could not find its needed files. In general FAMOUSO's           )
$(info make-system is able to determine its root directory automatically.              )
$(info Thus, you have three options:                                                   )
$(info  -First, you don't define FAMOUSO_INSTALLDIR and let the make-system do the job.)
$(info  -Second, you define the FAMOUSO_INSTALLDIR environment variable.               )
$(info  -Third, you start make as follows "make FAMOUSO_INSTALLDIR=famouso-root-dir".  )
$(info )
$(error FAMOUSO's root directory is given wrong)
endif

# Alle Makefiles in der richtigen Reihenfolge laden
#
# Definition des Targets und zusaetzlicher Einstellungen lesen
include $(FAMOUSOROOTDIR)/make/config.mk

# einige Definition lesen, die immer gelten
include $(FAMOUSOROOTDIR)/make/global.mk
include $(FAMOUSOROOTDIR)/make/sources.mk

#default compiler definition
include $(FAMOUSOROOTDIR)/make/buildtools.mk

# Definition lesen, die fuer spezielle Targets gelten
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/defs.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/sources.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/gcc.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(COMPILER)/tags.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(COMPILER)/additional_defs.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(COMPILER)/sources.mk
-include $(FAMOUSOROOTDIR)/make/$(PLATFORM)/$(COMPILER)/gcc.mk

include $(FAMOUSOROOTDIR)/make/rules.mk
