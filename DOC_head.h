/*
 * DOC_head.h
 *
 * Created: 6/4/16 7:50:59 AM
 *  Author: Roger
 */ 


#ifndef DOC_HEAD_H_
#define DOC_HEAD_H_

#define freq4M

#include <stdio.h>

#ifdef freq8M

#define F_CPU 8000000
#define timerroll 199

#endif

#ifdef freq4M

#define F_CPU 4000000
#define timerroll 99

#endif


//#define intensity 2   


#define u8 uint8_t
#define u16 uint16_t
/*
#define tog_tp1	{PORTB ^= 0x02;}
#define set_tp1	{PORTB |= 0x02;}
#define clr_tp1	{PORTB &= ~(0x02);}
get rid of these macros since tp1 has been re-appropriated as the 'voltmeter select' input
 */
 
 
//function declaration to set flag that will shut it down.
void setsleepstat (u8 newstat);	


#endif /* DOC_HEAD_H_ */