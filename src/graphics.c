/**********************************************************
*	Compilation:	avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -x assembler-with-cpp -c {file} -o {fileBasenameNoExtension}.o
*                   avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -o {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.o
*                   avr-objcopy -j .text -j .data -O ihex {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.hex
*	Flash:		avrdude -c arduino -P /dev/tty.usbmodem143101 -b 115200 -p atmega328p -U flash:w:{fileBasenameNoExtension}.hex:i
*	Dependencies:	
*	Environment: CrossPack-AVR on mac
*	contents and descriptions
*	@author Yifei Jing	yj10@hw.ac.uk
*	@date	16/11/19
**********************************************************/

#include "lcd.h"
#include "graphic.h"

// current id of obstacles to avoid collision
static int cid = 0;

// The image of the bird is of 5*4 pixes
// Use two GC memory region to store it
static char birdImage[4] = {0x12, 0x1F, 0x17, 0x02};


/*********              private functions               **********/

/*
 * Draw a bird on the screen, the height is the distance from
 * bottom
 */
static void setbird(uint8_t height, uint8_t x) // range from 0 to 12
{
    // Lcd4_Clear();
    for (char i = 0x00; ; ++i)
    {
        Lcd4_Set_CG_Addr(0x0F - i);
        if (i >= height && i <= height + 3)
            Lcd4_Write_CG(birdImage[i - height]);
        else Lcd4_Write_CG(0x00);
        if (i == 0x0F) break;
    }
    Lcd4_Set_Cursor(1,x);
    Lcd4_Write_Char(0);
    Lcd4_Set_Cursor(2,x);
    Lcd4_Write_Char(1);
}

/*
 * Draw an obstacle on the screen
 * @para x: position
 * @para y: the length from bottom
 * @para height: the blank between the upper and lower bound
 * @para id: which CG memory region to use [1,3]
 * @para shadow: whether to show the bird with obstacle [1,0]
 * @para birdheight: if show the bird, tell the height
 */
static void setobstacle(uint8_t x, uint8_t y, uint8_t height, uint8_t id, uint8_t shadow, uint8_t birdheight)
{
    uint8_t offset = id * 0x10;
    uint8_t dy = y, dheight = height, du = 15 - dy - dheight;
    uint8_t index = 0;
    uint8_t buffer[16];

    for (; index < du; ++index)
    {
        // Lcd4_Set_CG_Addr(offset + index);
        // Lcd4_Write_CG(0x0E);
        buffer[index] = 0x0E;
    }
    for (index = du; index < du + dheight; ++index)
    {
        // Lcd4_Set_CG_Addr(offset + index);
        // Lcd4_Write_CG(0x00);
        buffer[index] = 0x00;
    }
    for (index = du + dheight; index <= 0x0F; ++index)
    {
        // Lcd4_Set_CG_Addr(offset + index);
        // Lcd4_Write_CG(0x0E);
        buffer[index] = 0x0E;
    }
    if (shadow)
    {
        for (index = birdheight; index < birdheight + 4; ++index)
        {
            buffer[15 - index] |= birdImage[3 - (index - birdheight)];
        }
    }
    for (index = 0; index <= 0x0F; ++index)
    {
        Lcd4_Set_CG_Addr(offset + index);
        Lcd4_Write_CG(buffer[index]);
    }
    Lcd4_Set_Cursor(1, x);
    Lcd4_Write_Char(id * 2);
    Lcd4_Set_Cursor(2, x);
    Lcd4_Write_Char(id * 2 + 1);
}


// common apis exposed to user
void printBird(Object *bird)
{
    setbird(bird->y, bird->x);
}

void printObstacle(Object *obstacle, Object *bird)
{
    if (bird)
        setobstacle(obstacle->x, obstacle->y, obstacle->height, cid+1, 1, bird->y);
    else
        setobstacle(obstacle->x, obstacle->y, obstacle->height, cid+1, 0, 0);
    
    cid = (cid + 1) % 3;
}

void ClearScreen()
{
    Lcd4_Clear();
}

void printString(uint8_t lineNo,const char *s)
{
    Lcd4_Set_Cursor (lineNo, 0);
    Lcd4_Write_String (s);
}

void printChar(uint8_t a,uint8_t b,const char c)
{
    Lcd4_Set_Cursor (a, b);
    Lcd4_Write_Char (c);
}

void printDigits(uint8_t a,uint8_t b, const int num)
{
    Lcd4_Set_Cursor (a, b);
    if (num == 0)
    {
        Lcd4_Write_Char ('0');
        return;
    }

    int i = 1;
    while (num/i)
    {
        i *= 10;
    }
    int tmp = num;
    i /= 10;

    while(i)
    {
        Lcd4_Write_Char('0' + tmp/i);
        tmp -= (tmp/i) * i;
        i /= 10;
    }
}

void screenShiftRight()
{
    Lcd4_Shift_Right();
}

void screenShiftLeft()
{
    Lcd4_Shift_Left();
}