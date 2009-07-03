PCAN=/usr/lib/libpcan.so
PCANINC=/PCANINCLUDE
PCANLIB=$(shell test -f $(PCAN) && echo $(PCAN))

ADDITIONAL_CFLAGS 	+= -DLINUX -I$(PCANINC)
ADDITIONAL_LIBS     += $(PCANLIB) -lpthread
