/*
 * GPS.h
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_

#include "uart2.hpp"
extern Uart2& GPS_data;
void init_GPS_module();
double convert_to_degrees(double value);


#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_ */
