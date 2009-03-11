include ../../../make/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)
CXXFLAGS+= -O3

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))


FORMAT = ihex

# Target file name (without extension).
TARGET = RobotDemo

# Optimization level, can be [0, 1, 2, 3, s]. 0 turns off optimization.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s

ADDITIONAL_SRC = usart0.cc

#WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wsign-compare \
#		-Waggregate-return
#-Wunused

CXXFLAGS+= -D GCC_MEGA_AVR -I.  \
$(DEBUG_LEVEL) -O$(OPT) \
-funsigned-char -funsigned-bitfields -fshort-enums \
$(WARNINGS) \
-Wa,-adhlns=$(<:.cc=.lst)

# Optional linker flags.
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref



# Additional libraries

# Minimalistic printf version
#LDFLAGS += -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires -lm below)
#LDFLAGS += -Wl,-u,vfprintf -lprintf_flt

# -lm = math library
# LDFLAGS += -lm




# Programming support using avrdude. Settings and variables.

# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
#AVRDUDE_PROGRAMMER = stk200
AVRDUDE_PROGRAMMER = avr911
#AVRDUDE_PROGRAMMER = pcan


#AVRDUDE_PORT =		/dev/parport0   # programmer connected to serial device
AVRDUDE_PORT =		/dev/ttyUSB0   # programmer connected to serial device
#AVRDUDE_PORT =	/dev/pcan1   # programmer connected to CAN
#AVRDUDE_PORT = lpt1	# programmer connected to parallel port

AVRDUDE_WRITE_FLASH = -V -u -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE += -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_FLAGS += -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_FLAGS += -v -v




# ---------------------------------------------------------------------------

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size


# Programming support using avrdude.
AVRDUDE = avrdude

HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf


.phony: all clean program

all: $(TARGET)

# Program the device.
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)



$(TARGET).elf: $(TARGET)
	mv $< $@

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	avr-nm -n $< > $@

clean:
	@rm -rf $(filtered)

%:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@ $(ADDITIONAL_SRC) $(CXXFLAGS) $(LDFLAGS)

