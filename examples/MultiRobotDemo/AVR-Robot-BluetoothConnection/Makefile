C	= .cc
O	= .o

NAME	= DemoHannover

SRC	= $(basename $(wildcard *$(C)))
TARGET_C  = $(wildcard *$(C))
TARGET_O  = $(addsuffix $(O), $(SRC))

CTRL    = at90can128
INCLUDE = ../include
STK	= avrisp

CC      = avr-g++
OC      = avr-objcopy
LDFLAGS = -j .text -j .data

CFLAGS  = -g -O2 -Wall

AVRDUDE = avrdude

AVRDUDE_FLAGS =	-P /dev/parport0
AVRDUDE_FLAGS += -c $(STK)
AVRDUDE_FLAGS += -p $(CTRL)
AVRDUDE_FLAGS += -U f:w:$(NAME).hex:a

DEPENDFILE = .depend

all:	$(TARGET_O)
	$(CC) -o $(NAME).elf $(CFLAGS) -I $(INCLUDE) -mmcu=$(CTRL) $(TARGET_O)

dep:
	@echo "---- analyse dependencies -----" 
	#rm $(DEPENDFILE)
	$(CC) -MM $(TARGET_C) > $(DEPENDFILE)




%.o:%$(C)
	$(CC) -c $(CFLAGS) -I $(INCLUDE) -mmcu=$(CTRL)  $< -o $@


hex:	all
	@echo "generate properly output format"
	$(OC) $(LDFLAGS) -O ihex $(NAME).elf $(NAME).hex

program: hex
	@echo "Programing the device."
	$(AVRDUDE) $(AVRDUDE_FLAGS)

clean:
	@echo "Cleaning"
	@rm -rf $(TARGET_H) $(TARGET_O) $(NAME).elf *~

-include $(DEPENDFILE)
