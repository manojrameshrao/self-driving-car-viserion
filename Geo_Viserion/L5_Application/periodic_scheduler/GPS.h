/*
 * GPS.h
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_

#include "uart2.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.hpp"
#include "math.h"
struct cordinates{
    double latitude = 0;
    double longitude = 0;
};
extern Uart2& GPS_data;
extern char buffer[200];
void init_GPS_module();
void get_GPS_data();
double convert_to_degrees(double value);
void set_projection_data(double latitude, double longitude);
void set_checkpoint_data(double latitude, double longitude);
float get_bearing_angle();
#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_ */
