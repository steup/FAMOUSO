# -----------------------------------------------------------------------------
#
#                              M A K E F I L E
#
# -----------------------------------------------------------------------------
include ./make/Makefile

ALLSRC = $(SOURCES) $(TOOLSRC)

# -------------------------------------------------------------------------
# Namen der Unterverzeichnisse mit den Quelltexten
# (wird automatisch aus den Quelltextdateien ermittelt und vom Compiler
# benutzt, um die Quelltexte zu finden.)

VPATH = $(sort $(dir $(ALLSRC) ))

# Listen mit den Objektdateien, die beim Kompilieren entstehen:
# (werden automatisch aus den Quelltextdateinamen ermittelt)
# Objekt fuer die LIB
LIBOBJ   = $(addprefix $(MODULEDIR)/,$(notdir $(SOURCES:.cc=.o)))

# Objekt fuer die Tools
TOOLOBJ = $(addprefix $(MODULEDIR)/,$(notdir $(TOOLSRC:.cc=.o)))
# Ausfuehrbaren Tools
TOOLBIN = $(addprefix $(BINDIR)/,$(notdir $(TOOLSRC:.cc=)))

# Listen mit den Dependency-dateien, die beim Kompilieren entstehen:
# (werden automatisch aus den Quelltextdateinamen ermittelt)
DEPS = $(notdir $(ALLSRC:.cc=.d))
DEPSPRE = $(addprefix $(DEPENDDIR)/,$(DEPS))

# --------------------------------------------------------------------------
# Definition der Targets
.PHONY: all clean

all: $(LIBDIR) $(MODULEDIR) $(BINDIR) $(LIBCOSMIC) $(COMMANDS)

tools: $(LIBDIR) $(MODULEDIR) $(BINDIR) $(LIBCOSMIC) rmbin $(TOOLBIN)

$(LIBDIR):
	@mkdir -p $@

$(MODULEDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@

rmbin:
	@rm -f $(BINDIR)/*

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Objektdateien
$(MODULEDIR)/%.o : %.cc
	@$(RULEECHO) ; \
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Dependency-dateien
$(DEPENDDIR)/%.d : %.cc
	@$(RULEECHO) ; \
	$(CXX) -MM $(CXXFLAGS) $< | sed -e "s#.*:#$(MODULEDIR)\/&#" > $@

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der ausfuehrbaren Tools
$(BINDIR)/%: $(MODULEDIR)/%.o $(TOOLOBJ)
	@$(RULEECHO) ; \
	$(CXX) $(CXXOPTION) $(CXXFLAGS) -o $@ $< $(LIB) $(ADDITIONAL_LIBS)

# -------------------------------------------------
# Bauen der lib
$(LIBCOSMIC):$(LIBOBJ)
	@echo "generate lib *.o -> $(notdir $@)"
	@$(AR) $(ARFLAGS) $@ $(LIBOBJ)
	@$(RANLIB) $@

clean:	
	@rm -f $(MODULEDIR)/*.[oO] $(LIBLOADER) $(DEPENDDIR)/*.d $(BINDIR)/*
	@find . -name \*~ -exec rm -f {} \;
	@find . -name "#*#" -exec rm -f {} \;

distclean: clean
	@rm -rf $(LIBBASE) $(MODDIRBASE) $(BINDIRBASE) ./doc/html

-include $(DEPSPRE)


