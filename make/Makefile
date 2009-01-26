# zunaechst bestimmen, wo wir ueberhaupt installiert sind
ifeq ($(INSTALLDIR),)
ifeq ($(findstring /famouso/,$(shell pwd)/),/famouso/)

MW_SEARCH_COMMAND := \
while test ! -d famouso; do cd ..; done; \
pwd

INSTALLDIR := $(shell $(MW_SEARCH_COMMAND))/famouso

endif
endif

# Alle Makefiles in der richtigen Reihenfolge laden
#
# Definition des Targets und zusaetzlicher Einstellungen lesen
include $(INSTALLDIR)/make/config.mk
include $(INSTALLDIR)/make/tags.mk

# einige Definition lesen, die immer gelten
include $(INSTALLDIR)/make/global.mk
include $(INSTALLDIR)/make/sources.mk

# Definition lesen, die fuer spezielle Targets gelten
-include $(INSTALLDIR)/make/$(PLATFORM)/$(COMPILER)/gcc.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/defs.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/sources.mk

include $(INSTALLDIR)/make/rules.mk


ifeq ($(CONFIG),avr)
ADDITIONAL_BUILDS=
ADDITIONAL_LIBS=
endif
