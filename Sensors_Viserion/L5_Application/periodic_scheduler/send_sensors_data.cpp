/*
 * send_sensors_data.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Miroslav Grubic
 */

#include "send_sensors_data.h"
#include "generated_Viserion.h"
#include "io.hpp"

void send_sensors_data(int left, int right, int middle){


    SENSORS_VALUES_t sensors_values = { 0 };

    sensors_values.SENSOR_left_in = left;
    sensors_values.SENSOR_right_in = right;
    sensors_values.SENSOR_middle_in = middle;

    if(dbc_encode_and_send_SENSORS_VALUES(&sensors_values))
        LE.on(4);
}

