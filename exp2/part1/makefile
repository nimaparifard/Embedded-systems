#
# @description  Makefile for compiling, linking and flashing code into microcontroller atmega16
#
# @author       Marian Hrinko
# @datum        06.097.2020
# @notes				Suffix Replacement within a macro: $(name:string1=string2)
#               For each word in 'name' replace 'string1' with 'string2'
#               For example $(DEPENDENCIES:.c=.o)
# @inspiration  https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#               https://wiki.hacdc.org/index.php/AVR_Makefile
#
# BASIC CONFIGURATION, SETTINGS
# ------------------------------------------------------------------
#
# Final file
TARGET      	= main
#
# Library directory
LIBDIR        = lib
#
# Type of microcontroller
DEVICE        = atmega16
#
# Frequency
FCPU          = 16000000
#
# Optimization
OPTIMIZE      = Os
#
# Type of compiler
CC            = avr-gcc
#
# Compiler flags
CFLAGS        = -g -Wall -DF_CPU=$(FCPU) -mmcu=$(DEVICE) -$(OPTIMIZE)
#
# Includes
INCLUDES      = -I.
#
# Libraries
LIBS          = -L$(LIBDIR)
#
# Object copy
OBJCOPY       = avr-objcopy
#
# Objcopy, create hex file flags 
# -R .eeprom -O ihex or -j .data -j .text -O ihex
OBJFLAGS    	= -j .data -j .text -O ihex
#
# Size of file
AVRSIZE       = avr-size
#
# Size flags
SFLAGS        = --mcu=$(DEVICE) --format=avr
#
# Target and dependencies .cpp
SOURCES      := $(wildcard *.cpp $(LIBDIR)/*.cpp)
#
# Target and dependencies .o
OBJECTS	      = $(SOURCES:.cpp=.o)

# AVRDUDE CONFIGURATION, SETTINGS
# -------------------------------------------------------------------

#
# AVRDUDE
AVRDUDE       = avrdude
#
# AVRDUDE DEVICE
AVRDUDE_MMCU  = m16
#
# AVRDUDE PORT
AVRDUDE_PORT  = /dev/ttyUSB0
#
# AVRDUDE PROGRAMMER
AVRDUDE_PROG  = usbasp
#
# AVRDUDE BAUD RATE
AVRDUDE_BAUD  = 19200
#
# AVRDUDE BAUD RATE
AVROBJ_FORMAT = ihex
#
# AVRDUDE FLAGS
AVRDUDE_FLAGS = -p $(AVRDUDE_MMCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROG) -b $(AVRDUDE_BAUD) -u -U

# 
# Create file to programmer
main: $(TARGET).hex
	
# 
# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) $(OBJFLAGS) $(TARGET).elf $(TARGET).hex
	$(AVRSIZE) $(TARGET).elf

# 
# Create .elf file
$(TARGET).elf:$(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET).elf

#
# Create object files
%.o: %.cpp
	 $(CC) $(CFLAGS) -c $< -o $@

# 
# Program avr - send file to programmer
flash: 
	$(AVRDUDE) $(AVRDUDE_FLAGS) flash:w:$(TARGET).hex:i

#
# Clean
clean: 
	rm -f $(OBJECTS) $(TARGET).elf $(TARGET).map

#
# Cleanall
cleanall: 
	rm -f $(OBJECTS) $(TARGET).hex $(TARGET).elf $(TARGET).map