/**********************************************************
*	Compilation:	avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -x assembler-with-cpp -c {file} -o {fileBasenameNoExtension}.o
*                   avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -o {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.o
*                   avr-objcopy -j .text -j .data -O ihex {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.hex
*	Flash:		avrdude -c arduino -P /dev/tty.usbmodem143101 -b 115200 -p atmega328p -U flash:w:{fileBasenameNoExtension}.hex:i
*	Dependencies:	
*	Environment: CrossPack-AVR on mac
*	contents and descriptions
*	@author Yifei Jing	yj10@hw.ac.uk
*	@date	09/11/19
**********************************************************/
#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "types.h"
#define MAX_DEPTH 16
#define MAX_X 16
#define MAX_SPEED 3
#define SCORE_ADDR 0xF0

static BaseIntu32 FPS = 0;

enum type
{
    BIRD = 0,
    OBSTACLE
};
typedef enum type Type;

enum status
{
    BEGIN = 0,
    PLAYING,
    END
};
typedef enum status Status;

/*
 * Currently, for this game, there are only
 * two kinds of objects, the bird will have 
 * x and y to indicate its coordinate, the y
 * of the obstacle is the length of it from 
 * bottom, then height will control the part 
 * the bird can move through
 * */
struct object
{
    BaseIntu8 x;
    BaseIntu8 y;
    BaseIntu8 height;
    Type type;
};
typedef struct object Object;

/* 
 * All the objects in the game will be stored
 * in the object list to enable checked for 
 * updates each time the tick event happens
 * */
struct obListItem
{
    Object *obj;
    struct obListItem *next;
    struct obListItem *pre;
};;
typedef struct obListItem ObListItem;


void process();
Object *CreateBird();
Object *CreateObstacle(BaseIntu8 x, BaseIntu8 y, BaseIntu8 height);
void DeleteObj(Object *obj);
void DeleteListItem(ObListItem *item);
void deleteAll();
ObListItem *CreateListItem(Object *obj);
void ProcessScreen();
BaseIntu8 CheckHeight(ObListItem *item);
void AddNewItem(ObListItem *Item);
void InitGame();
void InitObstacles();
void AddNewObstacle();
void GameStart();
void GameEnd();
void GameLooper(BaseIntu8 s);
void OnButtonPressed(BaseIntu8 s);
void setFPS(uint16_t newFPS);
void subtileshifter();

#endif