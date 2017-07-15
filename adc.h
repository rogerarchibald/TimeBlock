/*
 * adc.h
 *
 * Created: 6/1/16 8:06:44 PM
 *  Author: Roger
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC_init(void);
uint16_t read_ADC(void);
void check3V(void); //function to check if voltage is < 3V and if so, shut down.  To be used when running with battery to ensure I don't try to run if the LiPo goes below 3V for safety

void defcoef(u8 coefIndex); //can set the coefficient for ADC based on resistor divider values

#endif /* ADC_H_ */
