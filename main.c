/*
 * Doug-O-Clock.c
 *
 *
 *This is the first rev of firmware to drive the timer for Doug's plane-clock.  It's a 4x7-Segment display that will just give a simple timer to start at the push of a button.  
 *The inputs are button and an analog input to give an indication of battery voltage (could also use this as a general purpose analog input if the board is powered off of uUSB connector).  
 *
 * Created: 5/23/16 7:35:33 PM
 * Author : Roger
 */ 


#include <avr/io.h>
#include "DOC_head.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "timer.h"
#include "adc.h"


volatile u8 gotosleep = 0;


//servicing the ISR for button press PCINT0 to wake things up, nothing to do here.
ISR(PCINT0_vect){
	asm ("nop");
	tog_tp1
}



int main(void)
{ 
	
DDRD = 0xFF;	//portD drives the digits of the 7-segment display
PORTD = 0;		//initially kill output
DDRC = 0x1F;	//PC4 = divider_control, PC0-3 = pass p-fets for digits
PORTC = 0;
DDRB = 0xEE;	//PB0 = button_in, PB1 = TP1
PORTB = 0;		//all off.	
PCMSK0 |= 0x01;	//unmask PCINT0 which is the button.
timer0Init();
init_ADC();
sei();	
set_sleep_mode(SLEEP_MODE_IDLE);
	

	
	
	
	
  
    while (1) 
    {
	sleep_mode();	
		if(gotosleep){	//go to sleep is a flag that gets set wehn it's time to go to sleep.  
			shut_r_down();
		}
	
    }

}	//end of main


void setsleepstat (u8 newstat){
	gotosleep = newstat;
}