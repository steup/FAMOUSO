CC		= gcc
CXX		= g++
AR		= ar
ARFLAGS	= ru
RANLIB	= ranlib
LD		= $(CXX)
AS		= as

ADDITIONAL_LIBS		+= -lpthread
ADDITIONAL_CFLAGS 	+= -DLINUX -g
