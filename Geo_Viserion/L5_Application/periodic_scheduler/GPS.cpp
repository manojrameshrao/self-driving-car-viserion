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
double convert_to_degrees(double value)
{
    double minutes;
    int reference,degrees;
    //longi = value;
    degrees = value/100;
    reference = (int)(value / 100) ;
    reference = reference * 100;
    minutes = (value - reference)/60;
    printf("value: %f", degrees + minutes);
    return degrees + minutes;
}

void get_GPS_data()
{
    const char delimiter[2] = ",";
    bool status = false;
    double latitude,longitude;
    char * gps_data;
    char *gps_latitude;
    char *gps_longitude;
    /* Get the GPS data */
    GPS_data.gets(buffer,sizeof(buffer),0);
    char buff[100] = "jean,130,50,geee";
    int i=0;
    gps_data= strtok(buff, delimiter);
    while (gps_data != NULL)
    {
      gps_data = strtok (NULL, ",");
      i++;
      if(i==1)
          gps_latitude = gps_data;
      if(i==2)
      {
          gps_longitude = gps_data;
          break;
      }
    }
    printf ("gps lat:%s\n",gps_latitude);
    printf ("gps long:%s\n",gps_longitude);
}

