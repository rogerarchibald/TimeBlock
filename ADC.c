/*
 * ADC.c
 * Will read the voltage on ADC7 which is the batter voltage on display.  Using external 1.25V reference since the internal 1.1V reference is too sloppy to use this as a voltmeter in the future.
 *8MHZ clock input /x64 prescaler = 125Khz clock which is perfect
 * Created: 6/1/16 8:04:17 PM
 *  Author: Roger
 */ 

#include <avr/io.h>
#include "DOC_head.h"
#include "adc.h"
#include <util/delay.h>




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
	_delay_ms(70);	//discharge the filter cap down to the divider level
    }   //this causes teh display to go dead/glitch when I'm in voltmeter mode
  
	ADCSRA |= 0x10;	//clear interrupt flag before starting a conversion
	ADCSRA |= (1 << ADSC);  //start conversion
	while(!(ADCSRA & 0x10)){}	//kill time while waiting for conversion to end
    if(!voltmeter){
	PORTC &= ~(0x30);//kill the FET  and voltage reference to conserve battery
    ADCSRA &= 0x0F;  //disable the whole ADC but keep the prescaler settings for next time around
    
    }   //end of using wheter or not we're in 'voltmeter' mode to determine whether or not su use some of hte power-savng lines
    voltval = ADC;
	voltval *= 410;	//convert ADC value to hundreds of mV when we're running a /3.43 divider
    voltval /= 100;	//convert ADC value to mV
	return voltval;
    
	
}


/*
The multiplication of 'voltval' by some coefficient is as follows:
 
 10-bit ADC = range of 0-1024.  voltval coefficient = 1.225/1024 * divider.  if divider = /3.43, coefficient = 1.225/1024 * 3.43 = .00410 (avoiding floats by multiplying by 410 then dividing by 100).
 
 if divider is 24.3k -> 10k...

 

*/
