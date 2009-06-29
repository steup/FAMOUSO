PCAN=/usr/lib/libpcan.so
PCANINC=/usr/include
PCANLIB=$(shell test -f $(PCAN) && echo $(PCAN))
