#ifndef UTILS_H
#define UTILS_H
// #define DEBUG

#include <avr/io.h>
#include <stdlib.h>
#include "util/delay.h"

// As the delay function should has a definite constant as its parameter
// Choose the act as the wrapper
#define delay_ms(x) _delay_ms(x)


#define MAX_TASK 5

// Task list used for tick event
// As the ISA is belong the hardware layer
// The Task list should be defined in this layer
typedef struct taskItem
{
    void (*p)();
    int16_t cycle;
    int32_t delay;
    int32_t state;
} taskItem;

static taskItem *TaskList[MAX_TASK] = {NULL};

typedef enum interruptState
{
    ENABLED,
    DISABLED
} InterruptState;

void initIO();
uint8_t readButton();
InterruptState disableTimer();
InterruptState enableTimer();
void backToLastInter(InterruptState);
void Sleep();
void test_eeprom();
void sendString(char const *s);
void sendDigits(uint32_t d);
void sendNewLine();
void wirteEEPROM(void *p, uint8_t data);
uint8_t readEEPROM(void *p);
taskItem* createTask (void (*p)(), int16_t cycle, int16_t delay);
uint8_t addTask(taskItem *task);
void removeTask(uint8_t id);
void deleteTask(taskItem *task);
void changeTaskDelay(uint8_t id, uint16_t delay);
void testmemory();
void LEDOn();
void LEDOff();
uint16_t readFPS();
uint16_t readSpeed();


#define RIGHT 0x01
#define LEFT 0x02
#define UP 0x04
#define DOWN 0x08
#define A 0x10
#define B 0x20

#endif