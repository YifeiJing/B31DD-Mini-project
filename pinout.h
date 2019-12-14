#ifndef PINOUT_H
#define PINOUT_H
// WARNING: THIS SHOULD BE INCLUDED BEFORE lcd.h // 

// LCD
#define LCD0 PB3
#define LCD1 PB2
#define LCD2 PB1
#define LCD3 PB0
#define LCD_RS PB4
#define LCD_EN PB5
#define LCD_DDR DDRB
#define LCD_PORT PORTB

// BUTTON
#define BUTTON_L PD7
#define BUTTON_D PD6
#define BUTTON_R PD5
#define BUTTON_U PD4
#define BUTTON_A PD3
#define BUTTON_B PD2
#define BUTTON_DDR DDRD
#define BUTTON_PIN PIND

// FPS
#define FPS_IN PC0
#define FPS_DDR DDRC
#define SPEED_IN PC1
#define SPEED_DDR DDRC

#define LED PC5
#define LEDDDR DDRC
#define LEDPORT PORTC

#endif