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
    voltval *= 424;	//convert ADC value to hundreds of mV when running a /1.736 divider and a 2.5V reference
    //voltval *= 1725;	//convert ADC value to hundreds of mV when running a /7.06 divider and a 2.5V reference
    voltval /= 100;	//convert ADC value to mV
	return voltval;
    
	
}


/*
The multiplication of 'voltval' by some coefficient is as follows:
 
 10-bit ADC = range of 0-1024.  voltval coefficient = 2.5/1023 * divider.  if divider = /1.736, coefficient = 1.225/1023 * 1.76 = .00424 (avoiding floats by multiplying by 424 then dividing by 100).
 

 
 With the 2.5V reference, voltval coef = 2.5/1023 * divideby
 for Rtop = 24.3K/Rbot = 33k, divideby = 1.736.....0.00424
 for Rtop = 200k/Rbot = 33k, divideby = 7.06......0.01725
 

*/
