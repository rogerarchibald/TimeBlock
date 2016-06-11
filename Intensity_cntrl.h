//
//  Intensity_cntrl.h
//  DOC_xavr
//
//  Created by Roger on 2016-06-10.
//  Copyright © 2016 Roger. All rights reserved.
//

#ifndef Intensity_cntrl_h
#define Intensity_cntrl_h

#include <stdio.h>
#include <avr/eeprom.h>
#include "DOC_head.h"

//function prototypes

u8 get_intensity (void);    //read the initial intensity value from EEPROM
void set_intensity (u8 newintensity); //re-program the intensity value




#endif /* Intensity_cntrl_h */
