### makefile for project

LIB = ./include
DESTINATION = ./output
SRC = ./src
SOURCES = main.c src/flappybird.c src/graphics.c src/lcd.c
HEADERS = main.h pinout.h include/flappybird.h include/graphic.h include/lcd.h include/types.h
OBJECTS = output/main.o output/flappybird.o output/graphics.o output/lcd.o
TARGET = Game

MCU   = atmega328p
F_CPU = 16000000UL  
BAUD  = 9600UL

PROGRAMMER_TYPE = arduino
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -P /dev/tty.usbmodem143101

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(LIB)
CFLAGS = -Os -g -std=gnu99 -Wall
TARGET_ARCH = -mmcu=$(MCU)

# %.o: %.c $(HEADERS) Makefile
# 	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $(DESTINATION)/$@ $<;

output/main.o : main.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
output/lcd.o : src/lcd.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
output/flappybird.o : src/flappybird.c	$(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
output/graphics.o : src/graphics.c	$(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;
output/$(TARGET).elf : $(OBJECTS)
	$(CC) $(TARGET_ARCH) $^ -o $@
output/$(TARGET).hex : output/$(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
all: output/$(TARGET).hex

clean:
	rm -f $(wildcard output/*)
	
flash: out/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<
	
# $(TARGET).elf: $(OBJECTS)
# 	$(CC) $(TARGET_ARCH) $^ $(LIB) -o $(DESTINATION)/$@

# %.hex: %.elf
# 	 $(OBJCOPY) -j .text -j .data -O ihex $< $(DESTINATION)/$@

# %.lst: %.elf
# 	$(OBJDUMP) -S $< > $(DESTINATION)/$@

# all: $(TARGET).hex
