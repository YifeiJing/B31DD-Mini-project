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
#include "utils.h"

#include <stdlib.h>

#define VERTCENTER 2

static ObListItem *head, *tail;
static Object *Bird;
static Status gameStatus = BEGIN;
static BaseIntu16 score = 0;
static BaseIntu8 speed = 1;

/******************************************************************************/
/***************************** private functions ******************************/
/******************************************************************************/

/* -------------------- Signal Process ------------------------- */
static void onButtonPressed_L()
{
    switch (gameStatus)
    {
    case BEGIN:
        GameLooper(0);
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
        GameLooper(0);
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
        GameLooper(0);
        break;
    case PLAYING:
        speed = 1;
        if (Bird->y+3 != MAX_DEPTH - 1) Bird->y++;
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
        GameLooper(0);
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
        GameLooper(0);
        break;
    case PLAYING:
        speed = 1;
        if (Bird->y+3 != MAX_DEPTH - 1) Bird->y++;
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
        GameLooper(1);
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
        GameLooper(0);
        break;
    case PLAYING:
        break;
    case END:
        InitGame();
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
    printString(1, " A Flappy Bird Game!!!");
    printString(2, " Press any button to play!");
}

static void printEndMenu()
{
    ClearScreen();
    printString(1, "Your Score: ");
    printDigits(1, 12, score);
    BaseIntu16 highestscore = readEEPROM(SCORE_ADDR);
    highestscore |= (readEEPROM(SCORE_ADDR+1) << 8);
    if (score > highestscore)
    {
        wirteEEPROM(SCORE_ADDR, score);
        wirteEEPROM(SCORE_ADDR+1, score >> 8);
    }
    printString(2, "Highest:");
    printDigits(2, 9, highestscore);
    // delay_ms(4000);
    while(1)
    {
        uint8_t signal = readButton();
        if (signal)
        {
            delay_ms(200);
            break;
        }
    }
    printString(2, "PLay Again[A] or Quit[B]?");
}

static void update()
{
    process();
    ProcessScreen();
}

/******************************************************************************/

void subtileshifter()
{
    if (gameStatus == PLAYING)
    {
        // counter = 0;
        return;
    }

    // static uint8_t counter = 0;

    // if (counter == 20)
    //     counter = 0;

    // if (counter < 10)
    // {
    //     screenShiftLeft();
    // }
    // else if (counter >= 10)
    // {
    //     screenShiftRight();
    // }

    // counter++;
    disableTimer();
    screenShiftLeft();
    enableTimer();
}

void buttonPressTask()
{
    BaseIntu8 signal = readButton();
    if (signal)
        OnButtonPressed(signal);
}

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
            tmp->obj->y -= speed;
            speed = speed == MAX_SPEED ? MAX_SPEED : speed + 1;
            if (tmp->obj->y >= MAX_DEPTH)
                GameEnd();
        }
        else if (tmp->obj->type == OBSTACLE)
        {
            tmp->obj->x--;
            // if the current processing item has moved out of screen
            if (tmp->obj->x == 0xFF)
            {
                ObListItem *tmp2 = tmp->pre;
                tmp->pre->next = tmp->next;
                tmp->next->pre = tmp->pre;
                DeleteObj(tmp->obj);
                DeleteListItem (tmp);
                tmp = tmp2;
                // After deletion, add a new obstacle
                AddNewObstacle();
            }
            else if (tmp->obj->x == Bird->x)
            {
                CheckHeight(tmp);
            }
        }
    }
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

void setFPS(BaseIntu16 newFPS)
{
    FPS = 977 / newFPS;
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
            if (tmp->pre->obj == Bird && tmp->obj->x == Bird->x)
            {
                printObstacle (tmp->obj, Bird);
            }
            else if (tmp->obj->x >= MAX_X) return;
            else
            {
                printObstacle (tmp->obj, NULL);
            }
            
        }
    }
}

BaseIntu8 CheckHeight(ObListItem *item)
{
    if (item->obj->y >= Bird->y) 
    {
        GameEnd();
        return;
    }
    else
    {
        if ((item->obj->y + item->obj->height) < Bird->y + 3)
        {
            GameEnd();
            return;
        }
    }
    score += 10;
    LEDOn();
    addTask(createTask(LEDOff,1,500));
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
    disableTimer();
    printInit();
    srand(123456);
    LEDOff();
    head = CreateListItem(CreateBird());
    tail = head;
    Bird = head->obj;
    InitObstacles();
    addTask(createTask(update, -1, 560));
    addTask(createTask(subtileshifter, -1, 200));
    addTask(createTask(buttonPressTask, -1, FPS));
    GameStart();
}

void InitObstacles()
{
    BaseIntu8 height, y, x;

    // randomly create three obstacles
    // First make three interval for obstacle each
    BaseIntu8 interval = MAX_X / 3;

    // As the bird is at x = 0, we choose the strategy
    // that the first obstacle is at x = 3
    y = rand() % (MAX_DEPTH - 5);
    height = 5 + rand() % (MAX_DEPTH - 5 - y);  // the height can not be 0, give it an offset

    AddNewItem (CreateListItem (CreateObstacle (3, y, height)));

    for (int i = 1; i <= 3; ++i)
    {
        x = interval * i + rand() % interval;
        y = rand() % (MAX_DEPTH - 5);
        height = 5 + rand() % (MAX_DEPTH - 5 - y);
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
    BaseIntu8 x, y, height;
    BaseIntu8 const interval = MAX_X / 4;
    x = rand() % interval;
    if (MAX_X + x == tail->obj->x + 1) x++;
    y = rand() % (MAX_DEPTH - 5);
    height = 5 + rand() % (MAX_DEPTH - 5 - y);

#ifdef DEBUG    
    sendString("Creat new abstacle: x :");
    sendDigits(MAX_X + x);
    sendString(" y:");
    sendDigits(y);
    sendString(" height:");
    sendDigits(height);
    sendNewLine();
#endif
    AddNewItem (CreateListItem (CreateObstacle (MAX_X + x, y, height)));
}

void GameStart()
{
    gameStatus = BEGIN;
    BaseIntu8 signal = 0;
    BaseIntu8 shift = 0;

    disableTimer();
    printStartMenu();
    enableTimer();
}

void deleteAll()
{
    ObListItem *tmp = head;
    while (tmp != tail)
    {
        DeleteObj(tmp->obj);
        tmp = tmp->next;
        DeleteListItem(tmp->pre);
    }
    DeleteObj(tmp->obj);
    DeleteListItem(tmp);
}

void GameEnd()
{
    disableTimer();
    gameStatus = END;
    deleteAll();
    BaseIntu8 signal = 0;
    printEndMenu();
    enableTimer();
}

/*
 * Use signal s to indicate whether it is restarted
 */
void GameLooper(BaseIntu8 s)
{
    if (s)
    {
        printInit();
        head = CreateListItem(CreateBird());
        tail = head;
        Bird = head->obj;
        InitObstacles();
    }
    score = 0;
    gameStatus = PLAYING;
    ProcessScreen();
    BaseIntu8 signal = 0;
    enableTimer();
}

void OnButtonPressed(BaseIntu8 s)
{
    disableTimer();
    if (s & UP) onButtonPressed_U();
    if (s & DOWN) onButtonPressed_D();
    if (s & LEFT) onButtonPressed_L();
    if (s & RIGHT) onButtonPressed_R();
    if (s & A) onButtonPressed_A();
    if (s & B) onButtonPressed_B();
    enableTimer();
}