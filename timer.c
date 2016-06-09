//
//  timer.c
//  
//
//  Created by Roger on 2016-05-23.
//
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "DOC_head.h"
#include "timer.h"
#include "adc.h"

u8 numbers[] = {0xD7, 0x14, 0xE6, 0xB6, 0x35, 0xB3, 0xF3, 0x16, 0xF7, 0xB7, 0x00, 0xE1, 0xE1, 0xF0, 0xF1};    //array corresponding to bitfields for values 0-9.  index 10 is all zeros. 11-14 are letters 'batt'
static u8 digits[] = {0,0,0,0}; //these are actually the values of the 4 digits.  Will use these to point to values in array above to drive the LED cathode bjt's
static u8 placeholder = 0;  //will cycle from 0-3 and repeat.  Will determine which of the 4 P-ch FETS for the anodes is active.
static u8 butstat = 0;	//will use this as a boolean to check the stat of the button.
static u8 laststat = 0;	//will use this to check current vs previous status of the button to know if it has changed
static u8 fallarm = 0;	//when the button has been detected hi for a minimum time (5mS) will set this hi.  Will prevent bounce by 
static u8 clockstat = clockrun;	//enumerated in header file.  will tell timer overflow isr what/how to display.
static u8 turnoff_arm = 1;	//will kill this when reading the ADC and then re-arm upon wakeup.
static u16 cyclesinstat = 0;	//how many cycles have I been in this status


//initialize timer0 with a 200uS rollover to trigger an ISR
void timer0Init(void){
    OCR0A = timerroll;   //timerroll is defined in DOC_head.h depending on clock frequency
    TCCR0A = 0x02;  //set WGM for CTC mode with OCR0A as top
    TCCR0B = 0x02;  //set CS1 for /8 prescaler
    TIFR0 |= 0x02;  //clear the OCF0A overflow flag
    TIMSK0 |= 0x02; //Enable interrupts from Timer0 OverflowA
    
}




//Timer0 CompareA Interrupt
ISR(TIMER0_COMPA_vect)	{
    static u8 two_hun_mics = 0;
    static u16 two_milliseconds = 0;
	
	
	
	butstat = (PINB & 0x01);
	if(butstat == laststat){
		cyclesinstat ++;
	} else{	//count how many times I've been in this status
		cyclesinstat = 0;
	} //reset counter if the status has changed.
	if ((cyclesinstat > 50) && laststat && turnoff_arm ){
		fallarm = 1;
		clockstat = clockrun;
	}	//if the button is hi and it's been hi for 50 cycles (50 x 200uS = 10mS), set it up so the next falling edge will do something
	if(fallarm && !butstat){		//if the button was armed and a falling edge was detected
	//	tog_tp1
		fallarm = 0;
		timeequals0();
		clockstat = clockoff;
		}
		if(turnoff_arm && !butstat && (cyclesinstat >= 0x2710))	//if the button has been low for 2 seconds
		{
			two_milliseconds = 0;	//reset the counter so next phase will be triggered 1 second later.
			display_batt();
			clockstat = batdisp;
			turnoff_arm = 0;	//don't come back until this is re-armed at wakeup
		}
	laststat = butstat;	//make sure laststat is up to speed for next round


    two_hun_mics ++;
    if (two_hun_mics >= intensity){ //once the intensity level is reached will kill the active LED
        PORTC|= 0x0F;  //this will set the 4 LSB's of Port C, ensuring all anodes are off
    }   //end of checking intensity
    
    if (two_hun_mics >= 10){
        next_digit();   //need to write function to cycle the 4 LSB's of PORTC to ensure that we're walking through
        two_hun_mics = 0;   //reset this value for next round
        two_milliseconds ++;    //if I've made it through 20 cycles of 100uS, increment the 2mS cntr...Every 500 of these will increment the second count
		
    }   //end of 2mS cycle
    
	
    if(two_milliseconds >=500){
			if(clockstat == clockrun){
				inc_seconds();  
			}else if (clockstat == batdisp){	//end of if clockstat = clockrun
				display_volts (read_ADC());
				clockstat = voltdisp; 
			}else if(clockstat == voltdisp){
				if(butstat && (cyclesinstat >= 0x2710)){	//if the button is released and has been for more than two seconds, set flag to put this to sleep.  Will set a trigger to wake on button change, so need to ensure it's off
				setsleepstat(1);
				}
				}
		
		two_milliseconds = 0;
    }
	
}	//end of ISR



//this function will cycle the anodes of the diodes and turn the appropriate anode on (turning it off is accomplished in teh Timer0 overflow ISR.
void next_digit(void){

	u8 dig_array[] = {0x04, 0x02, 0x01, 0x08};  //array to cycle through the different anodes
    placeholder ++;
    if(placeholder >= 4){
        placeholder = 0;
    }
    PORTD = numbers[digits[placeholder]];   //set up bjts on cathodes.
	
	
 switch (clockstat){	//this switch will determine the colon/decimal place
	 case clockrun:
		if(digits[0] & 0x01){  //if we've got an odd number
			PORTD |= 0x08;
		}	//set the colon when we have an odd number
		
		break;
		
		case voltdisp:
		if (placeholder == 2){
			PORTD |= 0x08;
		}
		break;
		
	}	//end of switch

    PORTC &= ~(dig_array[placeholder]);    //enable the appropriate anode by clearing the Gate of the P-ch FET.

}   //end of next_digit function



//function to actually increment the time.
void inc_seconds(void){
	
    digits[0] ++;
    if(digits[0] >= 10){
        digits[0] = 0;
        digits[1]++;
    }
    if(digits[1] >= 6){
        digits[1] = 0;
         digits[2]++;
    }
    if(digits[2] >= 10){
        digits[2] = 0;
        digits[3] ++;
    }
    if(digits[3] >= 10){
        digits[3] = 0;
    }
}





void timeequals0 (void){
	for(int i = 0; i < 4; i++){
		digits[i] = 0;
	}
}



void shut_r_down(void){
	timeequals0();
	TCCR0B = 0;		//kill timer
	PORTD = 0;		//turn off NPNs
	PORTC |= 0x0F;	//turn off P-ch
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);	//going to deep sleep
	PCIFR |= 0x01;	//clear interrupt flag
	PCICR |= 0x01;	//enable Pin Change Interrupt
	sleep_mode();
	asm ("nop");
	PCICR = 0;	//disable PC Interrupt
	set_sleep_mode(SLEEP_MODE_IDLE);	//change back to IDLE
	clockstat = clockrun;	//arming for post-reset
	fallarm = 0;
	turnoff_arm = 1;
	cyclesinstat = 0;
	setsleepstat(0);
	TCCR0B |= 0x02;	//re-enable clock
}

//this just puts 'batt' across the display.  Will be called for 1 second prior to displaying the actual voltage.
void display_batt(void){
		for (int i = 11; i < 15; i ++){
			digits[(i-11)] = i;
		}
}



void display_volts (u16 adval){

//this is will display the voltage...want to play with displaying the 'batt'	
adval /= 10;		//drop to x.yz volts.  will allow me to use the bottom of the colon as decimal place 
	digits[0] = adval % 10;
	adval /= 10;
	digits [1] = adval %10;
	adval /= 10;
	digits [2] = adval %10;
	digits [3] = 10;	//10 is all off
}

