#ifndef UTILS_H
#define UTILS_H

#include <avr/io.h>


void init();
void initIO();
void initLCD();
void initButtons();
void initFPS();
void initUSART();
void initTimer();
uint8_t readButton();


#define RIGHT 0x01
#define LEFT 0x02
#define UP 0x04
#define DOWN 0x08
#define A 0x10
#define B 0x20

#endif