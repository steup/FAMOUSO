CC		= gcc
CXX		= g++
AR		= ar
ARFLAGS	= ru
RANLIB	= ranlib
LD		= ld
AS		= as

ADDITIONAL_LIBS		+= -lwsock32 -lws2_32
ADDITIONAL_CFLAGS	+= -DWIN32 -D_WIN32_WINNT=0x0501
