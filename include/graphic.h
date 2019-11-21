/**********************************************************
*	Compilation:	avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -x assembler-with-cpp -c {file} -o {fileBasenameNoExtension}.o
*                   avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -o {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.o
*                   avr-objcopy -j .text -j .data -O ihex {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.hex
*	Flash:		avrdude -c arduino -P /dev/tty.usbmodem143101 -b 115200 -p atmega328p -U flash:w:{fileBasenameNoExtension}.hex:i
*	Dependencies:	
*	Environment: CrossPack-AVR on mac
*	header for graphics
*	@author Yifei Jing	yj10@hw.ac.uk
*	@date	16/11/19
**********************************************************/
#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "flappybird.h"

void printBird(Object *bird);
void printObstacle(Object *obstacle, Object *bird);
void ClearScreen();
void printString(uint8_t lineNo,const char *s);
void printChar(uint8_t a,uint8_t b,const char c);
void printDigits(uint8_t a,uint8_t b, const int num);
void screenShiftRight();
void screenShiftLeft();

#endif