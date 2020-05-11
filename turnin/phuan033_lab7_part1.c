/*	Author: lab
 *  Partner(s) Name: karen
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#ifdef _SIMULATE_
#include <avr/io.h>
#include "simAVRHeader.h"
#include "io.h"
#include <avr/interrupt.h>
#endif
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn()
{
        TCCR1B = 0x0B;
        OCR1A = 125;
        TIMSK1 = 0x02;
        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;
        SREG |= 0x80;
}

void TimerOff()
{
        TCCR1B = 0x00;
}

void TimerISR()
{
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0)
        {
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M)
{
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

enum States{START, INIT, INCREMENT, DECREMENT, WAIT_HOLD1, WAIT_HOLD2, RESET} state;
unsigned char butt;
unsigned char i = 0x00;
unsigned char tmp = 0x00;
void Tick()
{
	butt = (~PINA & 0x03);

	switch(state)
	{
		case START:
			i = 0;
			tmp = 0;
			LCD_Cursor(1);
			LCD_WriteData('0');
			state = INIT;
			break;

		case INIT:
			if(butt == 0x01)
			{
				state = INCREMENT;
			}
			else if(butt == 0x02)
			{
				state = DECREMENT;
			}
			else if(butt == 0x03)
			{
				state = RESET;
			}
			break;

		case INCREMENT:
			state = WAIT_HOLD1; //check to see if its been hold down
			break;

		case WAIT_HOLD1:
			if(butt == 0x01)
			{
                        	if(i >= 10 && tmp < 9)
                        	{
                                	tmp++;
					i = 0;
                       		}
				i++;
                	        LCD_Cursor(1);
        	                LCD_WriteData(tmp + '0');
	                        

			}
			else
			{
				state = INIT;
			}
			break;

		case WAIT_HOLD2:
			if(butt == 0x02) 
			{
                        	if(i >= 10 && tmp > 0)
                        	{
                        	        tmp--;
					i = 0;
                	        }
				i++;
        	                LCD_Cursor(1);
	                        LCD_WriteData(tmp + '0');

			}
			else
			{
				state = INIT;
			}
			break;
				
				

		case DECREMENT:
			state = WAIT_HOLD2; //check if hold
			break;

		case RESET: //fix here
			state = START;
			break;

		default:
			state = START;
			break;

	}

	switch(state)
	{
		case START:
			break;
		
		case INIT:
			break;

		case INCREMENT:
			if(tmp < 9)
			{
				tmp++;
			}
			LCD_Cursor(1);
			LCD_WriteData(tmp + '0');
			break;


		case DECREMENT:
			if(tmp > 0)
			{
				tmp--;
			}
			LCD_Cursor(1);
			LCD_WriteData(tmp + '0');
			break;

		case RESET:
			tmp = 0;
			LCD_Cursor(1);
			LCD_WriteData(tmp + '0');
			break;
	}
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	TimerSet(100);
	TimerOn();

//	LCD_DisplayString(1, "Hello World");
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
