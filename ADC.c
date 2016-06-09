/*
 * ADC.c
 * Will read the voltage on ADC7 which is the batter voltage on display.  Use internal 1.1V reference and note that there's a divide-by-two input
 *8MHZ clock input /x64 prescaler = 125Khz clock which is perfect
 * Created: 6/1/16 8:04:17 PM
 *  Author: Roger
 */ 

#include <avr/io.h>
#include "DOC_head.h"
#include "adc.h"
#include <util/delay.h>




void init_ADC(void){
ADMUX = 0xC7;	//C is to select the internal 1.1V reference, 7 selects the input
ADCSRA = 0x06;	//set prescaler to /64
}




uint16_t read_ADC (void){
	uint32_t voltval = 0;
	PORTC |= 0x10;	//turn on divider return FET
	PORTD = 0;	//turn off all BJT's so that in the next line this delay of 60mS doesn't look like a glitch on display
	_delay_ms(60);	//discharge the filter cap down to the divider level (25mS seems about right, 30 is a buffer)
	ADCSRA |= 0x10;	//clear interrupt flag before starting a conversion
	ADCSRA |= 0xC0; //enable ADC and start conversion
	while(!(ADCSRA & 0x10)){}	//kill time while waiting for conversion to end
	PORTC &= ~(0x10);	//kill the FET to conserve battery
	voltval |= ADCL;
	voltval |= (ADCH << 8);
	voltval *= 322;	//convert ADC value to hundreds of mV when we're running a /3 divider
	voltval /= 100;	//convert ADC value to mV
	
	return voltval;	
	
	
}