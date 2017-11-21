/*
 * get_adc.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: Miroslav Grubic
 */

#include <stdint.h>
#include "get_adc.h"
#include "adc0.h"


int get_adc(int channel_number)
{
    return adc0_get_reading(channel_number);
}
