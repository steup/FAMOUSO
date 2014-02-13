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

RULEECHO     = echo "$(notdir $<) -> $@"
LIBS         := $(addprefix -l, ${LIBS})
LDPATHS      := $(addprefix -L, ${LDPATHS})
INCLUDES     := $(addprefix -I, ${INCLUDES})
DEPENDANCIES := $(wildcard ${BUILDDIR}/*.d)
OBJECTS      := $(foreach src,$(notdir $(basename ${SOURCES})),${BUILDDIR}/${src}.o)

vpath %.cc $(sort $(dir $(shell find ${SRCDIR} -type f -name *.cc)))
vpath %.c  $(sort $(dir $(shell find ${SRCDIR} -type f -name *.c)))
vpath %.S  $(sort $(dir $(shell find ${SRCDIR} -type f -name *.S)))

${BUILDDIR}/%.o: %.cc | ${BUILDDIR}
	@${RULEECHO}
	@${CXX} -MM -MT $@ ${CXXFLAGS} ${INCLUDES} $< -o $@.d
	@${CXX} -c ${CXXFLAGS} ${INCLUDES} $< -o $@

${BUILDDIR}/%.o: %.c | ${BUILDDIR}
	@${RULEECHO}
	@${CC} -MM -MT $@ ${CFLAGS} ${INCLUDES} $< -o $@.d
	@${CC} -c ${CFLAGS} ${INCLUDES} $< -o $@ 
  
${BUILDDIR}/%.o: %.S | ${BUILDDIR}
	@${RULEECHO}
	@${ASM} -MM -MT $@ $(ASMFLAGS) ${INCLUDES} $< -o $@.d
	@${ASM} -c ${ASMFLAGS} ${INCLUDES} $< -o $@

$(LIBFAMOUSO): $(OBJECTS) ${NEEDS} | ${LIBDIR}
	@echo "generate lib *.o -> $(notdir $@)"
	@$(AR) $(ARFLAGS) $@ $^
	@$(RANLIB) $@

${DIRS}: %:
	mkdir -p $@

-include ${DEPENDANCIES}
