/*
 * trigger_left_and_right.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: Miroslav Grubic
 */

#include "trigger_left_and_right.h"

#include "utilities.h"



void trigger_left_and_right(uint32_t const &count, GPIO &l_trigger, GPIO &r_trigger){

    if(count % 10 == 0){

        l_trigger.setHigh();
        r_trigger.setHigh();

        delay_us(25);

        l_trigger.setLow();
        r_trigger.setLow();
    }
}


