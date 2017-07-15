/*
 * Doug-O-Clock.c
 *
 *
 *This is the first rev of firmware to drive the timer for Doug's plane-clock.  It's a 4x7-Segment display that will just give a simple timer to start at the push of a button.  
 *The inputs are button and an analog input to give an indication of battery voltage (could also use this as a general purpose analog input if the board is powered off of uUSB connector).  
 *
 *
 *Went to a newer design board with a 2.5V reference for the ADC and jumpers to allow selection of voltmeter or timer.  In Timer mode when running off battery it's critical to disable hte BOD while sleeping or else the battery won't last long due to the BOD consumption.  With an 
 *AtMega328 this requires disabling the BOD (Extended fuse = 0xFF).  With an AtMega328P it's possible to run the BOD but disable it while in deep sleep to 
 *
 *
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
#include "intensity_cntrl.h"



volatile u8 gotosleep = 0;


//servicing the ISR for button press PCINT0 to wake things up, nothing to do here.
ISR(PCINT0_vect){
	asm ("nop");

}



int main(void)
{

DDRD = 0xFF;	//portD drives the digits of the 7-segment display
PORTD = 0;		//initially kill output
DDRC = 0x3F;	//PC5 = bias for ADC reference, PC4 = divider_control, PC0-3 = pass p-fets for digits
PORTC = 0;
DDRB = 0xEC;	//PB0 = button_in, PB1 = voltmeter_select in
PORTB = 0;		//all off.	
PCMSK0 |= 0x01;	//unmask PCINT0 which is the button.
initialize_intensity (get_intensity());
ADC_init();
timer0Init();
sei();
set_sleep_mode(SLEEP_MODE_IDLE);

	
    if(PINB & 0x02){
        set_voltmeter();
        defcoef(0); //define coefficient for dealing with the ADC readings, this is done in ADC
    }else{
        check3V(); //If I'm in timer mode check if voltage is below 3V and if so, shut the whole thing down...the LiPo doesn't like to go any lower.
        defcoef(1); //define coefficient for dealing with the ADC readings, this is done in ADC
    }
	
  
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
