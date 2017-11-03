/*
 * init_pins.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: Miroslav Grubic
 */


#include "init_pins.h"


void init_pins(GPIO &l_trigger, GPIO &r_trigger, GPIO &m_trigger){

    l_trigger.setAsOutput();
    l_trigger.setLow();

    r_trigger.setAsOutput();
    r_trigger.setLow();

    m_trigger.setAsOutput();
    m_trigger.setLow();

    LPC_PINCON->PINSEL1 |= (1 << 20); // ADC-3 is on P0.26, select this as ADC0.3
    LPC_PINCON->PINSEL3 |= (3 << 28); // ADC-4 is on P1.30, select this as ADC0.4
    LPC_PINCON->PINSEL3 |= (3 << 30); // ADC-5 is on P1.31, select this as ADC0.5
}

