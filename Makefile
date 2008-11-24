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
TOOLBIN = $(addprefix $(BINDIR)/,$(notdir $(TOOLSRC:.cc=$(SUFFIX))))

# Listen mit den Dependency-dateien, die beim Kompilieren entstehen:
# (werden automatisch aus den Quelltextdateinamen ermittelt)
DEPS = $(notdir $(ALLSRC:.cc=.d))
DEPSPRE = $(addprefix $(DEPENDDIR)/,$(DEPS))

# --------------------------------------------------------------------------
# Definition der Targets
.PHONY: all clean doc

all: $(LIBDIR) $(MODULEDIR) $(BINDIR) $(DEPENDDIR) $(ADDITIONAL_BUILDS) $(LIBFAMOUSO)

doc:
	doxygen ./doc/doxygen.conf

$(LIBDIR):
	@mkdir -p $@

$(MODULEDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@

$(DEPENDDIR):
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

# -------------------------------------------------
# Bauen der lib
$(LIBFAMOUSO): $(LIBOBJ)
	@echo "generate lib *.o -> $(notdir $@)"
	@$(AR) $(ARFLAGS) $@ $(LIBOBJ)
	@$(RANLIB) $@

include ./make/boost.mk

clean:
	@rm -rf $(MODULEDIR) $(LIBFAMOUSO) $(DEPENDDIR) $(BINDIR)

distclean:
	@rm -rf $(LIBBASE) $(MODDIRBASE) $(BINDIRBASE) $(DEPDIRBASE) ./doc/html ./doc/latex
	@rm -rf ./include/boost
	@find . -name \*~ -exec rm -f {} \;
	@find . -name "#*#" -exec rm -f {} \;

depend: $(DEPENDDIR) $(DEPSPRE)

ifneq ($(subst dist,,$(MAKECMDGOALS)),depend)
-include $(DEPENDDIR)/*
endif

