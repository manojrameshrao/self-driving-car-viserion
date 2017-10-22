/*
 * send_sensor_hartbeat.cpp
 *
 *  Created on: Oct 22, 2017
 *      Author: ja
 */

#include "send_sensor_hartbeat.h"
#include "generated_Viserion.h"

void send_sensor_hartbeat(void){
    HB_SENSORS_t sensor_hb = { 0 };
    sensor_hb.SENSOR_ALIVE = 1;

    dbc_encode_and_send_HB_SENSORS(&sensor_hb);
}



