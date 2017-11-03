/*
 * trigger_middle.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: ja
 */

#include "trigger_middle.h"
#include "utilities.h"


void trigger_middle(uint32_t const count, GPIO &m_trigger){

    if(count % 10 == 5){

        m_trigger.setHigh();
        delay_us(25);
        m_trigger.setLow();
    }
}


