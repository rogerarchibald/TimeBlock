/*
 * DOC_head.h
 *
 * Created: 6/4/16 7:50:59 AM
 *  Author: Roger
 */ 


#ifndef DOC_HEAD_H_
#define DOC_HEAD_H_


#include <stdio.h>


#define intensity 2    //Value from 0-9 that determines the intensity of the LEDs


#define u8 uint8_t
#define u16 uint16_t

#define tog_tp1	{PORTB ^= 0x02;}
#define set_tp1	{PORTB |= 0x02;}
#define clr_tp1	{PORTB &= ~(0x02);}

//function declaration to set flag that will shut it down.
void setsleepstat (u8 newstat);	


#endif /* DOC_HEAD_H_ */