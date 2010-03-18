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

# -----------------------------------------------------------------------------
#
#                              M A K E F I L E
#
# -----------------------------------------------------------------------------
include ./make/Makefile

ALLSRC = $(SOURCES)

# -------------------------------------------------------------------------
# Namen der Unterverzeichnisse mit den Quelltexten
# (wird automatisch aus den Quelltextdateien ermittelt und vom Compiler
# benutzt, um die Quelltexte zu finden.)

VPATH = $(sort $(dir $(ALLSRC) ))

# Listen mit den Objektdateien, die beim Kompilieren entstehen:
# (werden automatisch aus den Quelltextdateinamen ermittelt)
# Objekt fuer die LIB
LIBOBJ   = $(addprefix $(MODULEDIR)/,$(notdir $(SOURCES:.cc=.o)))

# Listen mit den Dependency-dateien, die beim Kompilieren entstehen:
# (werden automatisch aus den Quelltextdateinamen ermittelt)
DEPS = $(notdir $(ALLSRC:.cc=.d))
DEPSPRE = $(addprefix $(DEPENDDIR)/,$(DEPS))

# --------------------------------------------------------------------------
# Definition der Targets
.PHONY: all clean doc deb

all: $(LIBDIR) $(MODULEDIR) $(DEPENDDIR) $(EXTERNALS) depend $(LIBFAMOUSO)

doc:
	doxygen doc/doxygen.conf
	cp ./doc/images/FAMOUSO.png ./doc/html
	cp ./tools/install-cygwin.bat ./doc/html

website:
	mkdir -p ./doc/www/docu
	doxygen ./doc/doxygen-website.conf
	cp ./doc/images/FAMOUSO.png ./doc/www/docu/online
	@echo "<!--#set var=\"base\" value=\"./../..\" -->" > ./doc/www/docu/online/base.shtml
	make -C ./doc/latex all
	cp ./doc/latex/refman.pdf ./doc/www/docu
	cp ./tools/install-cygwin.bat ./doc/www/docu/online


$(LIBDIR):
	@mkdir -p $@

$(MODULEDIR):
	@mkdir -p $@

$(DEPENDDIR):
	@mkdir -p $@

# -------------------------------------------------
# Bauen der lib
$(LIBFAMOUSO): $(LIBOBJ)
	@echo "generate lib *.o -> $(notdir $@)"
	@$(AR) $(ARFLAGS) $@ $(LIBOBJ)
	@$(RANLIB) $@

include ./make/externals.mk

clean:
	@rm -rf $(MODULEDIR) $(LIBFAMOUSO) $(DEPENDDIR)

distclean:
	@rm -rf $(LIBBASE) $(MODDIRBASE) $(DEPDIRBASE)
	@rm -rf ./doc/html
	@rm -rf ./doc/www/docu
	@rm -rf ./doc/latex
	@find . -name \*~ -exec rm -f {} \;
	@find . -name "#*#" -exec rm -f {} \;
	@make -C $(EXTERNALSDIR)/AVR distclean
	@debuild clean
	@rm -f $(INSTALLDIR)/debian

properclean: distclean
	@rm -rf $(EXTERNALSDIR)/Boost
	@rm -rf $(EXTERNALSDIR)/boost*
	@rm -rf $(EXTERNALSDIR)/include

debian:
	ln -f -s $(INSTALLDIR)/tools/debian $(INSTALLDIR)/debian

deb: all debian
	@debuild

depend: $(DEPENDDIR) $(DEPSPRE)

ifneq ($(subst dist,,$(MAKECMDGOALS)),depend)
-include $(DEPENDDIR)/*
endif

