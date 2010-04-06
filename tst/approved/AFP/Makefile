include ../../../make/Makefile
ADDITIONAL_CFLAGS+= -I../../../Bindings/include

SOURCES=$(wildcard *.cc)
BINS=$(SOURCES:.cc=)

.PHONY: all clean

ifneq ($(OPT),)
CXXFLAGS += -DNDEBUG -DFAMOUSO_DEBUG_DISABLE -DLOGGING_DISABLE
endif

ifneq ($(ADD_FLAGS),)
CXXFLAGS += $(ADD_FLAGS)
endif

all: $(BINS)


clean:
	rm -f $(BINS)

-include *-$(MACHINETYPE)-$(GCCVERSION).d
