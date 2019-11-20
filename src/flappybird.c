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

#include "flappybird.h"
#include "graphic.h"
#include "util/delay.h"
#include "main.h"

#include <stdlib.h>

#define VERTCENTER 2

static ObListItem *head, *tail;
static Object *Bird;
static Status gameStatus = BEGIN;
static BaseIntu16 score;

/******************************************************************************/
/***************************** private functions ******************************/
/******************************************************************************/

/* -------------------- Signal Process ------------------------- */
static void onButtonPressed_L()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        break;
    case END:
        break;
    
    default:
        break;
    }
}

static void onButtonPressed_R()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        break;
    case END:
        break;
    
    default:
        break;
    }
}

static void onButtonPressed_U()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        if (Bird->y != MAX_DEPTH - 1) Bird->y++;
        if (Bird->x == head->next->obj->x) 
        {
            CheckHeight(head->next);
            ProcessScreen();
        }
        else
        {
            printBird(Bird);
        }
        break;
    case END:
        break;
    
    default:
        break;
    }
}

static void onButtonPressed_D()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        break;
    case END:
        break;
    
    default:
        break;
    }
}

static void onButtonPressed_A()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        Bird->y++;
        if (Bird->x == head->next->obj->x) 
        {
            CheckHeight(head->next);
            ProcessScreen();
        }
        else
        {
            printBird(Bird);
        }
        break;
    case END:
        GameLooper();
        break;
    
    default:
        break;
    }
}

static void onButtonPressed_B()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper();
        break;
    case PLAYING:
        break;
    case END:
        GameStart();
        break;
    
    default:
        break;
    }
}
/* -------------- User Interfaces ---------------- */
static void printInit()
{
    ClearScreen();
    printString(1, "Initializing...");
}

static void printStartMenu()
{
    ClearScreen();
    printString(1, "Flappy Bird Game!!!");
    printString(2, "Press any button");
}

static void printEndMenu()
{
    ClearScreen();
    printString(1, "Score:");
    printDigits(1, 6, score);
    printString(2, "Again[A] or Quit[B]?");
}

/******************************************************************************/

/*
 * When a tick event happens, it will call this
 * function to perform the next state for each
 * objects in the list.
 * */
void process()
{
    if (gameStatus != PLAYING)
    {
        return;
    }
    
    ObListItem *tmp = head;

    for (; tmp != tail; tmp = tmp->next)
    {
        if (tmp->obj->type == BIRD)
        {
            tmp->obj->y--;
            if (tmp->obj->y < 0)
                GameEnd();
        }
        else if (tmp->obj->type == OBSTACLE)
        {
            tmp->obj->x--;
            // if the current processing item has moved out of screen
            if (tmp->obj->x == 0xFF)
            {
                tmp->pre = tmp->next;
                tmp->next->pre = tmp->pre;
                DeleteObj(tmp->obj);
                DeleteListItem (tmp);
                // After deletion, add a new obstacle
                AddNewObstacle();
            }
            else if (tmp->obj->x == Bird->x)
            {
                CheckHeight(tmp);
            }
        }
    }
    score += 10;
}

/*
 * bird will only be create at the start
 * of the game, and it will always be created
 * at the vertical center of the screen at the
 * left most
 * */
Object *CreateBird()
{
    Object *tmp = (Object*) malloc(sizeof(Object));
    tmp->type = BIRD;
    tmp->x = 0;
    tmp->y = 5;

    return tmp;
}

Object *CreateObstacle(BaseIntu8 x, BaseIntu8 y, BaseIntu8 height)
{
    Object *tmp = (Object*) malloc(sizeof(Object));
    tmp->type = OBSTACLE;
    tmp->x = x;
    tmp->y = y;
    tmp->height = height;

    return tmp;
}

void DeleteObj(Object *obj)
{
    free(obj);
}

void DeleteListItem(ObListItem *item)
{
    free(item);
}

ObListItem *CreateListItem(Object *obj)
{
    ObListItem *tmp = (ObListItem*) malloc(sizeof(ObListItem));
    tmp->obj = obj;
    return tmp;
}

/*
 *  Run over the object list and update graphics on screen
 *  This function should always be called after the process()
 */
void ProcessScreen()
{
    if (gameStatus != PLAYING)
    {
        return;
    }
    
    ObListItem *tmp = head;

    ClearScreen();
    for ( ; tmp != tail; tmp = tmp->next)
    {
        if (tmp->obj->type == BIRD)
        {
            printBird (tmp->obj);
        }
        else if (tmp->obj->type == OBSTACLE)
        {
            if (tmp->pre->obj == Bird)
            {
                printObstacle (tmp->obj, Bird);
            }
            else
            {
                printObstacle (tmp->obj, NULL);
            }
            
        }
    }
}

BaseIntu8 CheckHeight(ObListItem *item)
{
    if (item->obj->y >= Bird->y) GameEnd();
    else
    {
        if ((item->obj->y + item->obj->height) < Bird->y)
            GameEnd();
    }
    
}

/* add a new item to the end of the list */
void AddNewItem(ObListItem *Item)
{
    Item->pre = tail;
    tail->next = Item;
    tail = Item;
}

void InitGame()
{
    printInit();
    srand(123456);
    head = CreateListItem(CreateBird());
    tail = head;
    InitObstacles();

    GameStart();
}

void InitObstacles()
{
    int height, y, x;

    // randomly create three obstacles
    // First make three interval for obstacle each
    BaseIntu8 interval = MAX_X / 3;

    // As the bird is at x = 0, we choose the strategy
    // that the first obstacle is at x = 3
    y = rand() % (MAX_DEPTH - 1);
    height = 1 + rand() % (MAX_DEPTH - y);  // the height can not be 0, give it an offset

    AddNewItem (CreateListItem (CreateObstacle (3, y, height)));

    for (int i = 1; i < 3; ++i)
    {
        x = interval * i + rand() % interval;
        y = rand() % (MAX_DEPTH - 1);
        height = 1 + rand() % (MAX_DEPTH - y);
        AddNewItem (CreateListItem (CreateObstacle (x, y, height)));
    }
}

/*
 * Each time an obstacle move to the left to the screen, we 
 * need to delete it, and create a new one at the right to the screen
 * this function create a new obstacle with random y and height at MAX_X
 */
void AddNewObstacle()
{
    BaseIntu8 y, height;
    y = rand() % (MAX_DEPTH - 1);
    height = 1 + rand() % (MAX_DEPTH - y);

    AddNewItem (CreateListItem (CreateObstacle (MAX_X, y, height)));
}

void GameStart()
{
    gameStatus = BEGIN;
    BaseIntu8 signal = 0;

    printStartMenu();
    while(1)
    {
        signal = readButton();
        if (signal)
        {
            // _delay_ms(100);
            OnButtonPressed(signal);
        }
    }

    // this function never return
}

void GameEnd()
{
    gameStatus = END;
    BaseIntu8 signal = 0;
    printEndMenu();
    while(1)
    {
        signal = readButton();
        if (signal)
        {
            // _delay_ms(100);
            OnButtonPressed(signal);
        }
    }

    // this function never return
}

void GameLooper()
{
    score = 0;
    gameStatus = PLAYING;
    ProcessScreen();
    BaseIntu8 signal = 0;
    BaseIntu16 scaler = 0;
    while(1)
    {
        scaler++;
        if (scaler == 128)
        {
            counter++;
            if (counter == FPS)
            {
                tolerance++;
                if (tolerance == FPS/977)
                {
                    process();
                    ProcessScreen();
                    tolerance = 0;
                }
                counter = 0;

            }
            scaler = 0;
        }
        signal = readButton();
        if (signal)
        {
            // _delay_ms(100);
            OnButtonPressed(signal);
        }
    }
}

void OnButtonPressed(BaseIntu8 s)
{
    if (s & UP) onButtonPressed_U();
    if (s & DOWN) onButtonPressed_D();
    if (s & LEFT) onButtonPressed_L();
    if (s & RIGHT) onButtonPressed_R();
    if (s & A) onButtonPressed_A();
    if (s & B) onButtonPressed_B();
}

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