/**********************************************************
*	Compilation:	avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -x assembler-with-cpp -c {file} -o {fileBasenameNoExtension}.o
*                   avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -o {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.o
*                   avr-objcopy -j .text -j .data -O ihex {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.hex
*	Flash:		avrdude -c arduino -P /dev/tty.usbmodem143101 -b 115200 -p atmega328p -U flash:w:{fileBasenameNoExtension}.hex:i
*	Dependencies:	
*	Environment: CrossPack-AVR on mac
*	main file
*	@author Yifei Jing	yj10@hw.ac.uk
*	@date	20/11/19
**********************************************************/
#include "utils.h"
#include "flappybird.h"

int main()
{
    // Hardware layer initialization
    initIO();
    setFPS(30);

    // Object *tmp = CreateBird();
    // Object *obstcl = CreateObstacle(5,2,2);
    // printBird(tmp);
    // printObstacle(obstcl, NULL);
    // Software layer initialization
    InitGame();
    // GameStart();
    // Lcd4_Clear();
    // Lcd4_Set_Cursor(1,0);
    // Lcd4_Write_String("Christina");
    while(1);
    sleep_mode();
    // the return should never be reached
    return 0;
}