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

RULEECHO = echo "$(notdir $<) -> $@"

# How to compile a C++ file.
%.o:    %.cc
	@$(RULEECHO) ; \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CXXOPTIONS) -o $@ -c $<

# How to compile a C file.
%.o:    %.c
	@$(RULEECHO) ; \
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CCOPTIONS) -o $@ -c $<

# How to compile a C file.
%.o:    %.C
	@$(RULEECHO) ; \
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(CXXOPTIONS) -o $@ -c $<

# How to assemble a C++ file.
%.s:    %.cc
	@$(RULEECHO) ; \
	$(CXX) $(ASMOUT) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(CXXOPTIONS) -o $@ -c $<

# How to compile an assembler file.
%.o: %.S
	@$(RULEECHO) ; \
	$(CC) -c $< -o $@

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Objektdateien
$(MODULEDIR)/%.o : %.cc
	@$(RULEECHO) ; \
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Dependency-dateien
$(DEPENDDIR)/%.d : %.cc
	@$(RULEECHO) ; \
	$(CXX) -MM $(CXXFLAGS) $< -o $@
	@sed -e "s#.*:#$(MODULEDIR)\/&#;s#$(INCDIR)/boost/[a-z|A-Z|_|\.|\/|0-9]*##g;/^ *\\\/d;/[\s]/!d" $@ -i
	@sed -i -e '$$s/\ \\//' $@

# rule for creating the depend file and
# # for linking the application and give it
# # the correct name dependend of the platform
%:%.cc
	@$(RULEECHO) ; \
    $(CXX) -MM $(CXXFLAGS) $< -o $@-$(MACHINETYPE)-$(GCCVERSION).d ;
	@sed -e "s#.*:#$@:#;s#$(INCDIR)/boost/[a-z|A-Z|_|\.|\/|0-9]*##g;/^ *\\\/d;/[\s]/!d" $@-$(MACHINETYPE)-$(GCCVERSION).d -i
	@sed -i -e '$$s/\ \\//' $@-$(MACHINETYPE)-$(GCCVERSION).d
	@sed -i '$$i\ $(LIBFAMOUSO) \\' $@-$(MACHINETYPE)-$(GCCVERSION).d
	@$(CXX) $< -o $@.foobar $(CXXFLAGS) $(LDFLAGS) ; \
	mv $@.foobar $(basename $@)$(SUFFIX)


