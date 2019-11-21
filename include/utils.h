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
void disableTimer();
void enableTimer();
void Sleep();
void test_eeprom();
void wirteEEPROM(void *p, uint8_t data);
uint8_t readEEPROM(void *p);

#define RIGHT 0x01
#define LEFT 0x02
#define UP 0x04
#define DOWN 0x08
#define A 0x10
#define B 0x20

#endif