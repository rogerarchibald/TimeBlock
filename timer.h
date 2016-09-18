//
//  timer.h
//  
//
//  Created by Roger on 2016-05-23.
//
//

#ifndef timer_h
#define timer_h






//function declarations:

void timer0Init(void);
void next_digit(void);

void inc_seconds(void);
void shut_r_down(void);
void timeequals0 (void);
void display_batt (void);
void display_volts (u16 adval);
void display_intensity (void);
void initialize_intensity (u8 intense);
void set_voltmeter(void);


//the status will determine what's displayed.  This is checked in the ISR before updating time and is updated based on button presses or cycling through the batt display.
enum statusflag {
	clockoff,
	clockrun,
	batdisp,
	voltdisp,
    intenset
	};



#endif /* timer_h */
