/*

 * GPS.cpp
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
**/

#include "GPS.h"
#include "io.hpp"
char buffer[200] = {0};
char satelite[3];

uint8_t no_sat_locked;
Uart2& GPS_data = Uart2::getInstance();
cordinates current, projection, checkpoint;
 /* Initializes the GPS module
 * No Function parameters
 *
 **/
//struct cordinates current;
//, projection, checkpoint;
//current.latitude = 0;
void init_GPS_module()
{
    GPS_data.init(115200,76,1);//Configure the GPS in pedestrian mode
   // current.latitude = 0;
}
double convert_to_degrees(double value)
{
    double minutes;
    int reference,degrees;
    degrees = value/100;
    reference = (int)(value / 100) ;
    reference = reference * 100;
    minutes = (value - reference)/60;
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
    char buff[200] = "$GNGGA,094727.20,3720.00458,N,12154.72293,W,2,12,0.82,44.8,M,-29.9,M,,0000*41";
    int i=0;
    /* Byte 47 and 48 holds the number of connected satellites */
    satelite[0]=buffer[46];
    satelite[1]=buffer[47];
    satelite[2]='\n';
    no_sat_locked = atoi(satelite);
    if(no_sat_locked>=3)
    {
        /* Turn ON Led 1 */
        LE.on(1);
    }
    else
    {
        /* Turn OFF led 1*/
        LE.off(1);
    }

    gps_data= strtok(buffer, delimiter);
    while (gps_data != NULL)
    {
      gps_data = strtok (NULL, ",");
      i++;
      if(i==2)
          gps_latitude = gps_data;
      if(i==4)
      {
          gps_longitude = gps_data;
          break;
      }
    }
    printf ("gps lat:%f\n",convert_to_degrees(atof(gps_latitude)));
    printf ("gps long:%f\n",convert_to_degrees(atof(gps_longitude)));
}

