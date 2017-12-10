/*

 * GPS.cpp
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
**/
 /*Add dependent headers*/
#include "GPS.h"
#include "generated_Viserion.h"

/*Global definitions*/
char buffer[200] = {0};
char satelite[3];
uint8_t no_sat_locked;
char * format = NULL;
Uart2& GPS_data = Uart2::getInstance();
cordinates current, projection, checkpoint;
bool isNumberCheckPointsReceived;
//static bool allCordinatesReceived = false;
//static uint8_t no_checkpoint_reached = 0;
static unsigned int checkpoint_index =0;
static unsigned int sendCordinate = false;
static unsigned int allCheckpointsCorrectlyReceived = false;
SEND_GEO_NOT_READY_t geo_not_ready;
SEND_COORDINATES_t current_coordinates;

struct checkpoints{
     double latitude =0;
     double longitude =0;
}checkpoints_BT[20];


/* variable to receive CAN message */
//can_msg_t can_received_message;
dbc_msg_hdr_t can_received_message_header;

/* communication with BT-moudle */
SEND_NO_OF_CHECKPOINTS_t no_of_checkpoints;
COORDINATES_SENT_t cordinates_sent;
SEND_R_COORDINATES_t checkpoints;
MASTER_START_CMD_t start;
ALL_CHECKPOINTS_RECEIVED_t all_received;

 /* Initializes the GPS module
 * No Function parameters
 *
 **/
void init_GPS_module()
{
    /*Configure the GPS in pedestrian mode*/
    GPS_data.init(115200,76,1);
    LE.off(4);
    LE.off(2);
    //GPS_ready = false;
}

/* function: convert_to_degrees
 * Convert the DDDMM.MMMMM format to decimal degrees
 * value[in]: coordinates to be converted to decimal degrees
 * Returns :True(on success)/false(fail)
 */
double convert_to_degrees(double value)
{
    double minutes;
    int reference,degrees;
    degrees = value/100;
    reference = (int)(value / 100);
    reference = reference * 100;
    minutes = (value - reference)/60;
    return degrees + minutes;
}

/* function: get_GPS_data
 * Get current coordinates from GPS module
 */
void get_GPS_data()
{
    const char delimiter[2] = ",";
    char * gps_data = NULL;
    char *gps_latitude = NULL;
    char *gps_longitude = NULL;
    static uint8_t no_checkpoint_next = 0;


    /* Get the GPS data */
    GPS_data.gets(buffer,sizeof(buffer),0);
    char buff[200] = "$GNGGA,094727.20,3720.00458,N,12154.72293,W,2,12,0.82,44.8,M,-29.9,M,,0000*41";
    int i=0;
    //printf("GPS:%s\n", buffer);

    /* Byte 47 and 48 holds the number of connected satellites */
    satelite[0]=buffer[46];
    satelite[1]=buffer[47];
    satelite[2]='\n';
    no_sat_locked = atoi(satelite);

    /*Set the num of satelites on LED*/
    LD.setNumber(no_sat_locked);
    gps_data= strtok(buffer, delimiter);
    format = gps_data;
    //printf("format:%s\n", format);
    //GPS_ready = get_GNGGA_status(format) && get_satallites_status(no_sat_locked);
    if(GPS_ready())
    //if(1)
    {
        /* Turn ON Led 1 */
        LE.on(1);
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
        current.latitude = convert_to_degrees(atof(gps_latitude));
        current.longitude = (-1) * convert_to_degrees(atof(gps_longitude));
        //current.latitude = 37.3382082;
        //current.longitude = -121.8863286;
        set_projection_data(current.latitude,current.longitude);
        //ISSS chekpoint
        set_checkpoint_data(37.336601, -121.8811567);
        if(no_checkpoint_next == 0)
            set_checkpoint_data(checkpoints_BT[no_checkpoint_next].latitude,checkpoints_BT[no_checkpoint_next].longitude);
        if(checkpoint_reached())
            set_checkpoint_data(checkpoints_BT[no_checkpoint_next].latitude,checkpoints_BT[no_checkpoint_next].longitude);
            no_checkpoint_next++;

    }
    else
    {
        /* demo purpose */
        //printf("unlocked\n");
        /* Turn OFF led 1*/
        LE.off(1);
    }
    //printf ("gps lat:%f\n",convert_to_degrees(atof(gps_latitude)));
   // printf ("gps long:%f\n",convert_to_degrees(atof(gps_longitude)));
}

/* function: set_projection_data
 * Set projection coordinates based on current coordinates
 * latitude[in]: coordinates of current position
 * longitude[in]: coordinates of current position
 */
void set_projection_data(double latitude, double longitude)
{
    projection.latitude = latitude + 0.01;
    projection.longitude = longitude;
}

/* function: set_checkpoint_data
 * Set checkpoint coordinates based on bluetooth
 * latitude[in]: coordinates of current position
 * longitude[in]: coordinates of current position
 */
void set_checkpoint_data(double latitude, double longitude)
{
    checkpoint.latitude = latitude;
    checkpoint.longitude = longitude;
}

/* function: get_bearing_angle
 * Calculate the bearing angle using vector dot product
 * Bearing angle[out]: returns the bearing angle
 */
float get_bearing_angle()
{
    printf ("c_lat:%f\n",current.latitude);
    printf ("c_long:%f\n",current.longitude);
    //printf ("projection lat:%f\n",projection.latitude);
    //printf ("projection longi:%f\n",projection.longitude);
    printf ("cp_lat:%f\n",checkpoint.latitude);
    printf ("cp_long:%f\n",checkpoint.longitude);
    cordinates vector_CD, vector_CP;
    double dot_product = 0, mod_CD =0, mod_CP=0;
    float bearing_angle = 0;

    /*Get GPS data and set projection coordinates*/
    get_GPS_data();

    /*Vector coordinates of car position(current) to its destination (checkpoint)*/
    vector_CD.latitude = checkpoint.latitude - current.latitude;
    vector_CD.longitude = checkpoint.longitude - current.longitude;

    /*Vector coordinates of car position(current) to its projection (projection)*/
    vector_CP.latitude = projection.latitude - current.latitude;
    vector_CP.longitude = projection.longitude - current.longitude;

    /*Dot product of both Vectors CD & CP (CD . CP)*/
    dot_product = (vector_CD.latitude * vector_CP.latitude) + (vector_CD.longitude * vector_CP.longitude);

    /*Magnitude of vector_CD (|CD|)*/
    mod_CD = sqrt(pow(vector_CD.latitude,2) + pow(vector_CD.longitude,2));

    /*Magnitude of vector_CP (|CP|)*/
    mod_CP = sqrt(pow(vector_CP.latitude,2) + pow(vector_CP.longitude,2));

    /*Computes Arc-Cosine to find bearing angle and converts results to degrees.*/
    bearing_angle = acos(dot_product/(mod_CD*mod_CP)) * (180.0 / M_PI);

    /*Since range of arccos is [0,PI], we need to check for the position of current and
     * checkpoint longitudes to make the values in the range of (-180,180]
     */
    if(checkpoint.longitude < current.longitude)
        bearing_angle = bearing_angle*(-1);

    return bearing_angle;

}

/* function: get_GNGGA_status
 * Check $GNGGA format
 * bool[out]: returns true if $GNGGA format
 */
/*Check $GNGGA format*/
bool get_GNGGA_status(char * GPSFormat)
{
    if(strcmp(GPSFormat,"$GNGGA") == 0)
        return true;
    else
        return false;
}

/* function: get_satallites_status
 * Get satallites status
 * bool[out]: returns true if >=3 satellites locked
 */
bool get_satallites_status(uint8_t satellite)
{
    if( 3 <= satellite )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* function: destination_reached
 * Destination reached check
 * bool[out]: returns true if destination reached else false
 */
bool checkpoint_reached()
{
    if((current.latitude <= checkpoint.latitude + 0.00003) && (current.latitude >= checkpoint.latitude - 0.00003))
    {
        if((current.longitude <= checkpoint.longitude + 0.00003) && (current.longitude >= checkpoint.longitude - 0.00003))
        {
           //LE.toggle(4);
           return true;
        }
    }
    return false;
}

/* function: destination_reached
 * Send 1 if final destination has reached else 0
 * Bool[out]: returns true if reached else false
 */
bool destination_reached()
{
    if((current.latitude <= checkpoints_BT[checkpoint_index].latitude + 0.00003) && (current.latitude >= checkpoints_BT[checkpoint_index].latitude - 0.00003))
    {
        if((current.longitude <= checkpoints_BT[checkpoint_index].longitude + 0.00003) && (current.longitude >= checkpoints_BT[checkpoint_index].longitude - 0.00003))
        {
           //LE.toggle(4);
           return true;
        }
    }
    return false;

}

/* function: get_bearing_angle_haversine
 * Calculate the bearing angle using haversine
 * Bearing angle[out]: returns the bearing angle
 */
float get_bearing_angle_haversine()
{
   /*Haversine formula tan(theta) =  cos(lat2)*sin(dlon) /
                   (cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(dlon))*/

    float lat1,lat2,lon1,lon2,dlon,theta, numerator, denominator;

    /*Get GPS data and set projection coordinates*/
    get_GPS_data();

    /*Radian conversions*/
    lat1 = to_radians(current.latitude);
    lon1 = to_radians(current.longitude);
    lat2 = to_radians(checkpoint.latitude);
    lon2 = to_radians(checkpoint.longitude);
    dlon = lon2 - lon1;

    /*calculate the numerator and denominator for haversine formula*/
    numerator = (cos(lat2)*sin(dlon));
    denominator = (cos(lat1)*sin(lat2)) - (sin(lat1)*cos(lat2)*cos(dlon));

    /*Calculate bearing angle*/
    theta = atan2(numerator, denominator);
    return to_degrees(theta);

}

/* function: to_radians
 * Degrees to radians
 * Degree value[in]: input the degree value
 * Radian value[out]: returns the radian angle
 */
float to_radians(float value)
{
    return (value * TO_RAD);
}

/* function: to_radians
 * Radians to degrees
 * Radian value[in]: input the radian angle
 * Degree value[out]: return the degree value
 */
float to_degrees(float value)
{
    return (value / TO_RAD);
}

/* function: GPS_ready
 * Check if the GPS is ready to communicate
 * Degree value[out]: return 0 or 1 based on output
 */
bool GPS_ready()
{
    return get_GNGGA_status(format) && get_satallites_status(no_sat_locked);
}

/* function: send_current_cordinates
 * Send coordinates if flag is true else send not ready signal
 */
void send_current_cordinates(bool flag)
{
    if(flag)
    {
        /* Send msg 400 to send the current GPS coordinate*/
        current_coordinates.SEND_LATTITUDE = current.latitude;
        current_coordinates.SEND_LONGITUDE = current.longitude;
        dbc_encode_and_send_SEND_COORDINATES(&current_coordinates);
    }

    else
    {
        /*  Send msg 406 to send GPS_NOT_READY signal*/
        geo_not_ready.GPS_NOT_READY = 0;
        dbc_encode_and_send_SEND_GEO_NOT_READY(&geo_not_ready);
    }
    sendCordinate = false;
}

/* function: send_all_chekpoints_received
 * Send all_chekpoints_received signal to master with 1 if flag is true else send with 0
 */
void send_all_chekpoints_received(bool flag)
{
    if(flag)
    {
        /*  Send msg 404 with data as 1*/
        all_received.GEO_CHECKPOINTS_READY = 1;
        dbc_encode_and_send_ALL_CHECKPOINTS_RECEIVED(&all_received);
    }
    else
    {
        /*  Send msg 404 with data as 0*/
        all_received.GEO_CHECKPOINTS_READY = 0;
        dbc_encode_and_send_ALL_CHECKPOINTS_RECEIVED(&all_received);
    }

}
/* function: GPS_ready
 * Check if the GPS is ready to communicate
 * Degree value[out]: return 0 or 1 based on output
 */
bool GPS_receive_data_processing(can_msg_t can_received_message)
{
    //static bool isAllCheckPointsReceived = false;
    static bool start_flag = false;
    switch(can_received_message.msg_id)
    {
        /*  BT sends get_start_cordinate to receive the current GPS coordinates*/
        case 500:
            if(GPS_ready())
                sendCordinate = true;
            /* Send msg 400 or 406 based on sendCordinate flag*/
            send_current_cordinates(sendCordinate);
            break;

        /*  Receive all the checkpoints from BT and store them in array*/
        case 503:
             dbc_decode_SEND_R_COORDINATES(&checkpoints,can_received_message.data.bytes,&can_received_message_header);
             u0_dbg_printf("%lf",checkpoints.SET_LATTITUDE);
             u0_dbg_printf("%lf",checkpoints.SET_LONGITUDE);

             (checkpoints_BT[checkpoint_index]).latitude = checkpoints.SET_LATTITUDE;
             (checkpoints_BT[checkpoint_index]).longitude = checkpoints.SET_LONGITUDE;
             checkpoint_index++;
             break;

         /*  Receive number of checkpoints to the destination from BT module*/
         case 504:
             dbc_decode_SEND_NO_OF_CHECKPOINTS (&no_of_checkpoints,can_received_message.data.bytes,&can_received_message_header);
             printf("number of check points received: %d",no_of_checkpoints.BT_NO_OF_COR);

             if(no_of_checkpoints.BT_NO_OF_COR == checkpoint_index)
                 allCheckpointsCorrectlyReceived = true;

             /* Take action based on allCheckpointsCorrectlyReceived flag*/
             send_all_chekpoints_received(allCheckpointsCorrectlyReceived);
             break;

        /*  Receive start from master which will send the angles*/
        case 81:
            start_flag = true;
            break;

        default:
            break;
    }
    return start_flag;

}

void extract()
{
    /* uint32_t raw;
         raw  = ((uint32_t)((can_received_message.data.bytes[0]))); ///< 8 bit(s) from B0
         raw |= ((uint32_t)((can_received_message.data.bytes[1]))) << 8; ///< 8 bit(s) from B8
         raw |= ((uint32_t)((can_received_message.data.bytes[2]))) << 16; ///< 8 bit(s) from B16
         raw |= ((uint32_t)((can_received_message.data.bytes[3]) & 0x3f)) << 24; ///< 6 bit(s) from B24
         checkpoints.SET_LATTITUDE = ((raw * 1e-06) + (-180));
         raw  = ((uint32_t)((can_received_message.data.bytes[3] >> 6) & 0x03)); ///< 2 bit(s) from B30
         raw |= ((uint32_t)((can_received_message.data.bytes[4]))) << 2; ///< 8 bit(s) from B32
         raw |= ((uint32_t)((can_received_message.data.bytes[5]))) << 10; ///< 8 bit(s) from B40
         raw |= ((uint32_t)((can_received_message.data.bytes[6]))) << 18; ///< 8 bit(s) from B48
         raw |= ((uint32_t)((can_received_message.data.bytes[7]) & 0x0f)) << 26; ///< 4 bit(s) from B56
         checkpoints.SET_LONGITUDE = ((raw * 1e-06) + (-180));*/

}


