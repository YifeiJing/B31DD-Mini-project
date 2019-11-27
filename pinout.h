#ifndef PINOUT_H
#define PINOUT_H
// WARNING: THIS SHOULD BE INCLUDED BEFORE lcd.h // 

// LCD
#define LCD0 PC0
#define LCD1 PC1
#define LCD2 PC2
#define LCD3 PC3
#define LCD_RS PC4
#define LCD_EN PC5
#define LCD_DDR DDRC

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
#define FPS_DDR DDRD

#define LED PB5
#define LEDDDR DDRB
#define LEDPORT PORTB

#endif