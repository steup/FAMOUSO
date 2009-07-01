PCAN=/usr/lib/libpcan.so
PCANINC=/usr/include
PCANLIB=$(shell test -f $(PCAN) && echo $(PCAN))

ADDITIONAL_CFLAGS   += -I$(PCANINC)
ADDITIONAL_LIBS     += $(PCANLIB)

