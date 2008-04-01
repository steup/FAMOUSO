CC		= i386-mingw32-gcc
CXX		= i386-mingw32-g++
AR		= i386-mingw32-ar
ARFLAGS	= ru
RANLIB	= i386-mingw32-ranlib
LD		= i386-mingw32-ld
AS		= i386-mingw32-as

ADDITIONAL_LIBS		:= -lws2_32
ADDITIONAL_CFLAGS	:= -DWIN32 -DNDEBUG
