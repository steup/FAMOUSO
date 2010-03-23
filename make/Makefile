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

# If INSTALLDIR isn't defined, the FAMOUSO root directory is automatically determind
ifeq ($(INSTALLDIR),)
# get the path of this makefile
this_makefile_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
# erase the trailing /make/ in order to get the correct root of FAMOUSO
INSTALLDIR := $(patsubst %/make/,%,$(this_makefile_dir))
endif

ifeq ($(wildcard $(INSTALLDIR)/make/config.mk),)
# INSTALLDIR is defined wrong
$(info )
$(info The INSTALLDIR you gave to FAMOUSO's make-system is wrong. The make-system  )
$(info could not find its needed files. In general FAMOUSO's make-system is able   )
$(info to determine its root directory automatically. Thus, you have three options:)
$(info     - First, you don't define INSTALLDIR and let the make-system do the job.)
$(info     - Second, you define the INSTALLDIR environment variable.               )
$(info     - Third, you start make as follows "make INSTALLDIR=famouso-root-dir".  )
$(info )
$(error FAMOUSO's root directory is given wrong)
endif

# Alle Makefiles in der richtigen Reihenfolge laden
#
# Definition des Targets und zusaetzlicher Einstellungen lesen
include $(INSTALLDIR)/make/config.mk

# einige Definition lesen, die immer gelten
include $(INSTALLDIR)/make/global.mk
include $(INSTALLDIR)/make/sources.mk

#default compiler definition
include $(INSTALLDIR)/make/buildtools.mk

# Definition lesen, die fuer spezielle Targets gelten
-include $(INSTALLDIR)/make/$(PLATFORM)/defs.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/sources.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/gcc.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/$(COMPILER)/tags.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/$(COMPILER)/additional_defs.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/$(COMPILER)/sources.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/$(COMPILER)/gcc.mk

include $(INSTALLDIR)/make/rules.mk
