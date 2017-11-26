/*
 * trigger_sensors.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: Miroslav Grubic
 */

#include "trigger_sensors.h"
#include "utilities.h"


/**
 * Since this function is called from 100Hz (10ms) task it will trigger
 * sensors every 150ms but not at the same time.
 * Sensor is triggered by setting GPIO output pin on our board to high for about 25us
 * @count number that shows how many times periodic function period_100Hz() is called
 * @l_trigger, m_trigger, r_trigger object of GPIO class defining pin to trigger left sensor
 */
void trigger_sensors(uint32_t const &count, GPIO &l_trigger, GPIO &m_trigger, GPIO &r_trigger, GPIO &b_trigger){

    if(count % 15 == 0){

        l_trigger.setHigh();
        delay_us(25);
        l_trigger.setLow();
    }

    else if(count % 15 == 5){

        m_trigger.setHigh();
        delay_us(25);
        m_trigger.setLow();
    }

    else if(count % 15 == 10){

        r_trigger.setHigh();
        delay_us(25);
        r_trigger.setLow();
    }

    if(count % 5 == 0){

        b_trigger.setHigh();
        delay_us(25);
        b_trigger.setLow();
    }
}

