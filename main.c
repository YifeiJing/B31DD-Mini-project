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

// io.h should be included first! //
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "pinout.h"
#include "flappybird.h"
#include "lcd.h"
#include "main.h"

static uint32_t FPS = 0;
static uint32_t counter = 0;

/* 
 * The system plans not to use interrupts to implement 
 * reactions, the pooling strategy will handle all of them.
*/
void initIO()
{
    initButtons();
    initLCD();
    initFPS();
    initUSART();
    initTimer();
    sei();
}

void initButtons()
{
    BUTTON_DDR &= ~(_BV(BUTTON_A)| _BV(BUTTON_B)| _BV(BUTTON_U)
    | _BV(BUTTON_D)| _BV(BUTTON_L)| _BV(BUTTON_R));
}

void initTimer()
{
    // prescaler 64
    TCCR0B |= ((1<<CS01)|(1<<CS00));
    // use overflow interrupt
    // basic interrupt freq: 16000000/64/256=977Hz
    TIMSK0 |= ((1<<TOIE0));
}

void initLCD()
{
#define D4 eS_PORTB0
#define D5 eS_PORTB1
#define D6 eS_PORTB2
#define D7 eS_PORTB3
#define RS eS_PORTB4
#define EN eS_PORTB5

    LCD_DDR |= (_BV(LCD0)|_BV(LCD1)|_BV(LCD2)|_BV(LCD3)|_BV(LCD_RS)|_BV(LCD_EN));
    Lcd4_Init();
}

void initFPS()
{
    FPS_DDR &= ~(_BV(FPS_IN));
    // Configure ADC,  ADC0
    ADMUX |= (1 << REFS0);
    // ADC enable, prescaler 128
    ADCSRA |= ((1 << ADEN)|(1 << ADPS2)|(1 << ADPS2)|(1 << ADPS0));
}

void initUSART()
{
    // set baud rate
    UBRR0 = 16000000/16/9600 - 1;
    // Enable receive and transmit
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    // set frame format
    // 8 bits data, 1 stop bit, 0 parity bit
    // Asychronous USART
    // Default setting
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t readButton()
{
    uint8_t tmp = 0;
    if (BUTTON_PIN & _BV(BUTTON_A)) tmp |= A;
    if (BUTTON_PIN & _BV(BUTTON_B)) tmp |= B;
    if (BUTTON_PIN & _BV(BUTTON_L)) tmp |= LEFT;
    if (BUTTON_PIN & _BV(BUTTON_R)) tmp |= RIGHT;
    if (BUTTON_PIN & _BV(BUTTON_D)) tmp |= DOWN;
    if (BUTTON_PIN & _BV(BUTTON_U)) tmp |= UP;

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

uint16_t readADC()
{
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    return ADC;
}

void setFPS(uint16_t newFPS)
{
    FPS = 977 / newFPS;
}

ISR(TIMER0_OVF)
{
    counter++;
    if (counter == FPS)
    {
        process();
        ProcessScreen();
        counter = 0;
    }
}

int main()
{
    // Hardware layer initialization
    initIO();
    setFPS(30);

    // Software layer initialization
    InitGame();

    sleep_mode();
    // the return should never be reached
    return 0;
}