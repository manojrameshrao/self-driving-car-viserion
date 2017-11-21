/*
 * calculate_distance.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: Miroslav Grubic
 */

#include <stdint.h>
#include "calculate_distance.h"
#include "adc0.h"



int adc1 = 0, adc2 = 0, adc3 = 0;
float voltage1 = 0, voltage2 = 0, voltage3 = 0;

/**
 * Microcontroller has 12bit ADC and is powered with 3.3V so divide with 2^12=4096
 * to get each step value
 * */
const float adc_step = 3.3 / 4096;

/**
 * Sensors outputs analog voltage with a scaling factor of (Vcc/512) per inch. A supply of 5V yields ~9.8mV/in.
 */
const float voltage_scaling = 0.009765625;

/**
 * Function that calculate distance of the obstacle.
 * @l, r and m used to store calculated distances
 */
void calculate_distance(int &l, int &r, int &m){

    adc1 = adc0_get_reading(3);
    adc2 = adc0_get_reading(4);
    adc3 = adc0_get_reading(5);

    voltage1 = adc1 * adc_step;
    voltage2 = adc2 * adc_step;
    voltage3 = adc3 * adc_step;

    l = voltage1 / voltage_scaling;
    r = voltage2 / voltage_scaling;
    m = voltage3 / voltage_scaling;

    /**
     * Since we noticed that sensors sometimes return values higher than 254 (upper border of sensors range)
     * there is need to deal with that case. Value 25 is assigned because it is our threshold when master is informing
     * motor what to do. Value 25 will not change value returned from filter since list is 4 elements long
     */
    if(l > 254) l = 25;
    if(m > 254) m = 25;
    if(r > 254) r = 25;
}


