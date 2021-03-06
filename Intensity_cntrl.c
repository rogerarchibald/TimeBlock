//
//  Intensity_cntrl.c
//  DOC_xavr
//
//  Created by Roger on 2016-06-10.
//  Copyright © 2016 Roger. All rights reserved.
//

#include "Intensity_cntrl.h"



//update the intensity value stored in address 10 of EEPROM
void set_intensity(u8 newintensity){
    eeprom_write_byte ((uint8_t*)10, newintensity);
}


//read back the intensity stored in address 10 of EEPROM, ensure it's no greater than 9 and return it.
u8 get_intensity(void){
u8 tempintense = eeprom_read_byte((uint8_t*)10); //read byte 10 of EEPROM, which is where I'll store the intensity
    if(tempintense > 9){
        tempintense = 9;
    }

    return tempintense;
}