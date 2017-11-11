/*
 * trigger_right.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: Miroslav Grubic
 */


#include "trigger_right.h"
#include "utilities.h"


/**
 * Since this function is called from 100Hz (10ms) task it will trigger
 * right sensor every 150ms but not at the same time when middle and left sensors.
 * Sensor is triggered by setting GPIO output pin on our board to high for about 25us
 * @count number that shows how many times periodic function period_100Hz() is called
 * @r_trigger object of GPIO class defining pin to trigger middle sensor
 */
void trigger_right(uint32_t const &count, GPIO &r_trigger){

    if(count % 15 == 10){

        r_trigger.setHigh();
        delay_us(25);
        r_trigger.setLow();
    }
}

