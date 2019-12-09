/**********************************************************
*	Compilation:	avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -x assembler-with-cpp -c {file} -o {fileBasenameNoExtension}.o
*                   avr-gcc -Wall -Os -DF_CPU=16000000 -mmcu=atmega328p -o {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.o
*                   avr-objcopy -j .text -j .data -O ihex {fileBasenameNoExtension}.elf {fileBasenameNoExtension}.hex
*	Flash:		avrdude -c arduino -P /dev/tty.usbmodem143101 -b 115200 -p atmega328p -U flash:w:{fileBasenameNoExtension}.hex:i
*	Dependencies:	
*	Environment: CrossPack-AVR on mac
*	contents and descriptions
*	@author Yifei Jing	yj10@hw.ac.uk
*	@date	13/11/19
**********************************************************/

#include "utils.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "pinout.h"
#include "lcd.h"

static InterruptState current_interState = DISABLED;

// private decalarations
void initLCD();
void initButtons();
void initFPS();
void initUSART();
void initLED();
void initTimer();
uint16_t readADC(uint8_t);

/* 
 * The system plans not to use interrupts to implement 
 * reactions, the pooling strategy will handle all of them.
*/
void initIO()
{
    initButtons();
    initLCD();
    // initFPS();
#ifdef DEBUG
    initUSART();
#endif
    initTimer();
    initLED();
    enableTimer();
}

void initButtons()
{
    BUTTON_DDR &= ~(_BV(BUTTON_A)| _BV(BUTTON_B)| _BV(BUTTON_U)
    | _BV(BUTTON_D)| _BV(BUTTON_L)| _BV(BUTTON_R));
}

void initTimer()
{
    // prescaler 64
    TCCR2B |= ((1<<CS22));
    // use overflow interrupt
    // basic interrupt freq: 16000000/64/256=977Hz
    TIMSK2 |= (1<<TOIE2);
}

void initLCD()
{
    LCD_DDR |= (_BV(LCD0)|_BV(LCD1)|_BV(LCD2)|_BV(LCD3)|_BV(LCD_RS)|_BV(LCD_EN));
    Lcd4_Init();
    Lcd4_Clear();
}

void initLED()
{
    LEDDDR |= _BV(LED);
}

void initFPS()
{
    FPS_DDR &= ~(_BV(FPS_IN));
    SPEED_DDR &= ~(_BV(SPEED_IN));
    // Configure ADC,  ADC0
    ADMUX |= _BV(REFS0);
    // ADC enable, prescaler 128
    ADCSRA |= (_BV(ADEN)|_BV(ADPS2)|_BV(ADPS2)|_BV(ADPS0));
}

void initUSART()
{
    // set baud rate
    UBRR0 = 16000000/16/9600 - 1;
    // Enable receive and transmit
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    // set frame format
    // 8 bits data, 1 stop bit, 0 parity bit
    // Asychronous USART
    // Default setting
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

uint8_t readButton()
{
    // use a register data to farther accelerate access
    register uint8_t pin_status = BUTTON_PIN;
    uint8_t tmp = 0;
    if (pin_status & _BV(BUTTON_A)) tmp |= A;
    if (pin_status & _BV(BUTTON_B)) tmp |= B;
    if (pin_status & _BV(BUTTON_L)) tmp |= LEFT;
    if (pin_status & _BV(BUTTON_R)) tmp |= RIGHT;
    if (pin_status & _BV(BUTTON_D)) tmp |= DOWN;
    if (pin_status & _BV(BUTTON_U)) tmp |= UP;

    return tmp;
}

void transmitByte(uint8_t data)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;
}

uint8_t receiveByte()
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

void sendString(char const *s)
{
    while (*s)
    {
        transmitByte(*s);
        s++;
    }
    
}

void sendDigits(uint32_t d)
{
    if (d == 0)
    {
        transmitByte('0');
        return;
    }
    int i = 1;
    while (d/i)
    {
        i *= 10;
    }
    int tmp = d;
    i /= 10;

    while(i)
    {
        transmitByte('0' + tmp/i);
        tmp -= (tmp/i) * i;
        i /= 10;
    }
}

void sendNewLine()
{
    transmitByte(0x0a);
}

uint16_t readFPS()
{
    return readADC(0);
}

uint16_t readSpeed()
{
    return readADC(1);
}

uint16_t readADC(uint8_t select)
{
    if (select)
    {
        ADMUX |= _BV(MUX0);
    }
    else
    {
        ADMUX &= ~_BV(MUX0);
    }
    
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    return ADC;
}

ISR(TIMER2_OVF_vect)
{
    current_interState = DISABLED;
    // Deal with tasks
    for (uint8_t i = 0; i < MAX_TASK; ++i)
    {
        if (TaskList[i])
        {
            if (TaskList[i]->cycle > 0)
            {
                if (TaskList[i]->state == TaskList[i]->delay)
                {
                    TaskList[i]->p();
                    TaskList[i]->cycle--;
                    TaskList[i]->state = 0;
                }
                else
                {
                    TaskList[i]->state++;
                }
                
            }
            // -1 for infinite cycles
            else if (TaskList[i]->cycle == -1)
            {
                if (TaskList[i]->state == TaskList[i]->delay)
                {
                    TaskList[i]->p();
                    TaskList[i]->state = 0;
                }
                else
                {
                    TaskList[i]->state++;
                }
            }
            else
            {
                deleteTask(TaskList[i]);
                TaskList[i] = NULL;
            }   
        }
    }
    current_interState = ENABLED;
}

void LEDOn()
{
    LEDPORT |= _BV(LED);
}

void LEDOff()
{
    LEDPORT &= ~_BV(LED);
}

InterruptState disableTimer()
{
#ifdef DEBUG
    sendString("DisableTimer called");
    sendNewLine();
#endif
    if (current_interState == DISABLED) return DISABLED;
    TCCR2B = 0;
    cli();
    current_interState = DISABLED;
#ifdef DEBUG
    sendString("Timer disabled");
    sendNewLine();
#endif
    return ENABLED;
}

void backToLastInter(InterruptState lastState)
{
    if (lastState == ENABLED) enableTimer();
    else disableTimer();
}

InterruptState enableTimer()
{
#ifdef DEBUG
    sendString("Enable Timer called");
    sendNewLine();
#endif
    if (current_interState == ENABLED) return ENABLED;
    TCCR2B |= ((1<<CS22));
    TCNT2 = 0;
    current_interState = ENABLED;
#ifdef DEBUG
    sendString("Timer enabled");
    sendNewLine();
#endif
    sei();
    return DISABLED;
}

void Sleep()
{
    sleep_mode();
}

void test_eeprom()
{
    // eeprom_write_byte(0x0F, 100);
    eeprom_write_byte(0xF0, 0);
    eeprom_write_byte(0xF1, 0);

    while (1)
    {
        char a = receiveByte();
        if (a == 'a') transmitByte(eeprom_read_byte(0x0F));
        else if (a == 'b') transmitByte(eeprom_read_byte(0xF1));
        else
        {
            transmitByte(eeprom_read_byte(0xF2));
        }
        
    }
    
}

void wirteEEPROM(void *p, uint8_t data)
{
    eeprom_write_byte(p, data);
}

uint8_t readEEPROM(void *p)
{
    return eeprom_read_byte(p);
}

taskItem* createTask (void (*p)(), int16_t cycle, int16_t delay)
{
    taskItem *tmp = (taskItem*) malloc(sizeof(taskItem));
    tmp->p = p;
    tmp->cycle = cycle;
    tmp->delay = delay;
    tmp->state = 0;

    return tmp;
}

uint8_t addTask(taskItem *task)
{
    for (uint8_t i = 0; i < MAX_TASK; ++i)
    {
        if (!TaskList[i]) 
        {
            TaskList[i] = task;
            return i;
        }
    }

    return 0xFF;
}

void deleteTask(taskItem *task)
{
    free(task);
}

void testmemory()
{
    sendString("Initialize a task item");
    transmitByte(0x0a);
    taskItem* task = (taskItem *) malloc(sizeof(taskItem));
    task->p = testmemory;
    task->cycle = 1;
    task->delay = 10;
    task->state = 0;

    int tmp = (int) task;
    while (tmp/10)
    {
        transmitByte('0' + tmp%10);
        tmp = tmp/10;
    }
    
    free(task);
    sendString("Free memory success");
    transmitByte(0x0a);
    tmp = (int) task;
    while (tmp/10)
    {
        transmitByte('0' + tmp%10);
        tmp = tmp/10;
    }
}

void removeTask(uint8_t id)
{
    deleteTask(TaskList[id]);
    TaskList[id] = NULL;
}
