/*
 * adc.h
 *
 * Created: 6/1/16 8:06:44 PM
 *  Author: Roger
 */ 


#ifndef ADC_H_
#define ADC_H_


void init_ADC(void);	//set up reference, mux etc but don't turn on ADC for battery power.
uint16_t read_ADC(void);


#endif /* ADC_H_ */