#ifndef LCD_H
#define LCD_H
//LCD Functions Developed by electroSome


#define D4 LCD0
#define D5 LCD1
#define D6 LCD2
#define D7 LCD3
#define RS LCD_RS
#define EN LCD_EN
#define PORT LCD_PORT


#include <avr/io.h>
#include<util/delay.h>

void pinChange(char a, char b);
void Lcd4_Port(char a);
void Lcd4_Cmd(char a);
void Lcd4_Clear();
void Lcd4_Set_Cursor(char a, char b);
void Lcd4_Init();
void Lcd4_Write_Char(char a);
void Lcd4_Write_String(char *a);
void Lcd4_Shift_Right();
void Lcd4_Shift_Left();
void Lcd4_Set_CG_Addr(char a);
void Lcd4_Write_CG(char a);

#endif