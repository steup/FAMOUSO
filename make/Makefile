# zunaechst bestimmen, wo wir ueberhaupt installiert sind
ifeq ($(INSTALLDIR),)
ifeq ($(findstring /cosmic/,$(shell pwd)/),/cosmic/)

MW_SEARCH_COMMAND := \
while test ! -d cosmic; do cd ..; done; \
pwd

INSTALLDIR := $(shell $(MW_SEARCH_COMMAND))/cosmic

endif
endif

# Alle Makefiles in der richtigen Reihenfolge laden
#
# Definition des Targets und zusaetzlicher Einstellungen lesen
include $(INSTALLDIR)/make/config.mk

# einige Definition lesen, die immer gelten
include $(INSTALLDIR)/make/global.mk
include $(INSTALLDIR)/make/sources.mk

# Definition lesen, die fuer spezielle Targets gelten
-include $(INSTALLDIR)/make/$(PLATFORM)/gcc.mk
-include $(INSTALLDIR)/make/$(PLATFORM)/sources.mk


include $(INSTALLDIR)/make/rules.mk

COMMANDS=tools

ifeq ($(PLATFORM),avr)
COMMANDS=
endif
