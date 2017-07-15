/*
 * ADC.c
 * Will read the voltage on ADC7 which is the battery voltage on display.  Using external 2.5V reference since the internal 1.1V reference is too sloppy to use this as a voltmeter in the future.
 *4MHZ clock input /x64 prescaler = 64Khz clock for ADC
 * Created: 6/1/16 8:04:17 PM
 *  Author: Roger
 */ 

#include <avr/io.h>
#include "DOC_head.h"
#include "adc.h"
#include <util/delay.h>
#include "timer.h"
#include <avr/interrupt.h>

static u16 coef;


#define coefTimer ((2.5/1023)/(33/(33+24.3))*100000)
#define coefVoltMeter 1725 //compiler doesn't like the math, denomimator of divider is too big((2.5/1023)*100000)/(33/(33+200))




//this is called from main when determining if it's a voltmeter or a timer.  
void defcoef(u8 coefIndex){
    if(coefIndex){
    coef = coefTimer;
   }else{
        coef = coefVoltMeter;
   }
}

void ADC_init(void){
ADMUX = 0x07;	//0 is to select the external voltage reference, 7 selects the input
ADCSRA = 0x06;	//set prescaler to /64
}



uint16_t read_ADC (void){
	uint32_t voltval = 0;
    extern u8 voltmeter;
    PORTC |= 0x30;	//turn on divider return FET and ADC input bias
    ADCSRA |= (1 << ADEN);
    if(!voltmeter){
    PORTD = 0;	//turn off all BJT's so that in the next line this delay of 70mS doesn't look like a glitch on display
	_delay_ms(20);	//discharge the filter cap down to the divider level
    }   //this causes teh display to go dead/glitch when I'm in voltmeter mode
  
	ADCSRA |= 0x10;	//clear interrupt flag before starting a conversion
	ADCSRA |= (1 << ADSC);  //start conversion
	while(!(ADCSRA & 0x10)){}	//kill time while waiting for conversion to end
    if(!voltmeter){
	PORTC &= ~(0x30);//kill the FET  and voltage reference to conserve battery
    ADCSRA &= 0x0F;  //disable the whole ADC but keep the prescaler settings for next time around
    
    }   //end of using wheter or not we're in 'voltmeter' mode to determine whether or not su use some of hte power-savng lines
    voltval = ADC;
    voltval *= coef;//coefficient is determined in 'defcoef' function that's called by main depending on whether we're in counter mode or voltmeter mode.  The voltmeter coefficient assumes a 200k rtop and a 33k rbot.  These values can be modified if different resistors are installed.  The idea is to convert ADC counts to '10s of mV
    voltval /= 100;
	return voltval;
    
	
}




/*the flow will be as follows: when 'check3V' is called the 'two_milliseconds' variable will be at zero since it either will have just been initialized or it will have just been zeroed when rolling over intothe next second.  'check3V' will set up the ADC by turning on the ADC reference and the FET to ground the resistor divider, and enable an ADC interrupt.  When 'two_milliseconds' gets to 10 (20 milliseconds) will enable the ADC.  The ISR will turn everything off and if the ADC result indicates a battery voltage below 3V, will initiate shutdown

*/
ISR(ADC_vect){
    PORTC &= ~(0x30);//kill the FET  and voltage reference to conserve battery
    ADCSRA &= 0x07;  //disable the whole ADC and interrupt but keep the prescaler settings for next time around
    if (ADC < 707){setsleepstat(1);}
}




//will call this when running on battery.  IF the voltage turns out to be below 3V will shut things down.  Assuming the rtop = 24.3k/rbot = 33k divider and a 2.5V reference
//with a divider of 1.736, 3V in will equal 1.7277V on the ADC input...1.7277/2.5 = 0.691....0.691 * 1023 = 706.99...If ADC reading is below 707, will shut this down.
void check3V(void){
    PORTC |= 0x30;	//turn on divider return FET and ADC input bias

 }




