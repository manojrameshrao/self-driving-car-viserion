/*

 * GPS.cpp
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
**/


#include "stdio.h"
#include "stdlib.h"`

#include "GPS.h"

Uart2& GPS_data = Uart2::getInstance();

 /* Initializes the GPS module
 * No Function parameters
 *
 **/
void init_GPS_module()
{
    GPS_data.init(115200,76,1);//Configure the GPS in pedestrian mode
}

