/*
 * trigger_left.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: Miroslav Grubic
 */

#include <periodic_scheduler/init_and_trigger/trigger_left.h>
#include "utilities.h"


/**
 * Since this function is called from 100Hz (10ms) task it will trigger
 * left sensor every 150ms but not at the same time when middle and right sensors.
 * Sensor is triggered by setting GPIO output pin on our board to high for about 25us
 * @count number that shows how many times periodic function period_100Hz() is called
 * @l_trigger object of GPIO class defining pin to trigger left sensor
 */
void trigger_left(uint32_t const &count, GPIO &l_trigger){

    if(count % 15 == 0){

        l_trigger.setHigh();
        delay_us(25);
        l_trigger.setLow();
    }
}


