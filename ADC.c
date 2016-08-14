/*
 * ADC.c
 * Will read the voltage on ADC7 which is the batter voltage on display.  Use internal 1.1V reference and note that there's a divide-by-three input
 *8MHZ clock input /x64 prescaler = 125Khz clock which is perfect
 * Created: 6/1/16 8:04:17 PM
 *  Author: Roger
 */ 

#include <avr/io.h>
#include "DOC_head.h"
#include "adc.h"
#include <util/delay.h>


void ADC_init(void){
ADMUX = 0xC7;	//C is to select the internal 1.1V reference, 7 selects the input
ADCSRA = 0x06;	//set prescaler to /64
}



uint16_t read_ADC (void){
	uint32_t voltval = 0;
    clr_tp1
	PORTC |= 0x10;	//turn on divider return FET
	PORTD = 0;	//turn off all BJT's so that in the next line this delay of 70mS doesn't look like a glitch on display
    ADCSRA |= (1 << ADEN);
	_delay_ms(70);	//discharge the filter cap down to the divider level
    set_tp1
	ADCSRA |= 0x10;	//clear interrupt flag before starting a conversion
	ADCSRA |= (1 << ADSC);  //start conversion
	while(!(ADCSRA & 0x10)){}	//kill time while waiting for conversion to end
	PORTC &= ~(0x10);	//kill the FET to conserve battery
    voltval = ADC;
	voltval *= 550;	//convert ADC value to hundreds of mV when we're running a /5.12 divider
    voltval /= 100;	//convert ADC value to mV
    ADCSRA &= 0x0F;  //disable the whole ADC but keep the prescaler settings for next time around
	return voltval;
    
	
}


/*
The multiplication of 'voltval' by some coefficient is as follows:
 
 10-bit ADC = range of 0-1024.  voltval coefficient = 1.1/1024 * divider.  if divider = /3, coefficient = 1.1/1024 * 3 = .00322 (avoiding floats by multiplying by 322 then dividing by 100).
 
 if divider is 100k - 24.3k... 24.3k/124.3k = .195    1/.195 = 5.12   1.1/1024 * 5.12 = .0055 (avoid floats by multiplying by 550 then dividing by 100)



*/