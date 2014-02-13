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

.PHONY: forward all clean doc deb deb-clean

forward: all

FAMOUSOROOTDIR := .

include  ./make/global.mk
include  ./make/sources.mk
include  ./make/externalsRules.mk
-include ./make/$(PLATFORM)/sources.mk
-include ./make/$(PLATFORM)/$(FLAVOR)/sources.mk
include  ./make/rules.mk

all: ${LIBFAMOUSO} ${NEEDS}

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


clean:
	@rm -rf $(LIBFAMOUSO) $(BUILDDIR)

distclean: clean externalsclean
	@rm -rf ./build ./lib
	@rm -rf ./doc/html
	@rm -rf ./doc/www/docu
	@rm -rf ./doc/latex
	@make -C $(EXTERNALSDIR)/AVR distclean
