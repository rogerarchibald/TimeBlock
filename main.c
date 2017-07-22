/*
 * Doug-O-Clock.c
 *
 *
 *TimeBlock (Doug-O-Clock) is a timer based on Doug M's request for a simple timekeeper while flying.
 *The inputs are button and an analog input to give an indication of battery voltage (could also use this as a general purpose analog input if the board is powered off of uUSB connector).  
 *
 *
 *This has gone through a few iterations with different analog references and different board configurations/battery management options.  Current design uses a 2.5V reference for the ADC and jumpers to allow selection of voltmeter or timer.  In Timer mode it's assumed that I'm running a 903048 battery, which 
 *doesn't like to go below 3V.  The ADC will check battery voltage once/second and shut the whole thing down if it goes below 3V, to keep the battery happy.
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
initialize_intensity (get_intensity());  //read stored intensity value from EEPROM and then tell the Timer.c file what its value is
ADC_init(); //just set up the ADC prescaler and point the MUX at the ADC input from voltage divider.
timer0Init();
sei();  //enable interrupts
set_sleep_mode(SLEEP_MODE_IDLE);    //while running, will go to idle in between 200uS interrupts.  When turning off, the 'shut_r_down' function will change the sleep mode to deep sleep.

	//these lines determine if the jumper has been set to select voltmeter mode and set the ADC coefficent accordingly (at the time of writing this, I'm assuming that timer mode == a 24.3k/33k divider and voltmeter mode == 200k/33k divider).  If those values change, the predefined coefficients in ADC must be modified.
    if(PINB & 0x02){
        set_voltmeter();
        defcoef(0); //define coefficient for dealing with the ADC readings, this is done in ADC
    }else{
        check3V(); //If I'm in timer mode check if voltage is below 3V and if so, shut the whole thing down...the LiPo doesn't like to go any lower.
        defcoef(1); //define coefficient for dealing with the ADC readings, this is done in ADC
    } //end of determining where to go based on voltmeter configuration.  
	
  
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
