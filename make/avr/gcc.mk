CC		= avr-gcc
CXX		= avr-g++
AR		= avr-ar
ARFLAGS	= ru
RANLIB	= avr-ranlib
LD		= avr-ld
AS		= avr-as

MCU=at90can128
ADDITIONAL_CFLAGS += -DAVR -mmcu=$(MCU)
