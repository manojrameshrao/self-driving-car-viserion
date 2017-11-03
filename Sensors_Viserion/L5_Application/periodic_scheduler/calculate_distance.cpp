/*
 * calculate_distance.cpp
 *
 *  Created on: Nov 2, 2017
 *      Author: ja
 */

#include <stdint.h>
#include "calculate_distance.h"
#include "adc0.h"



int adc1 = 0, adc2 = 0, adc3 = 0;
float voltage1 = 0, voltage2 = 0, voltage3 = 0;

const float adc_step = 3.3 / 4096;
const float voltage_scaling = 0.009765625;

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
}


