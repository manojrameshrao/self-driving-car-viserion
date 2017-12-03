/*
 * receive_sensor_values.h
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#ifndef RECEIVE_SENSOR_VALUES_H_
#define RECEIVE_SENSOR_VALUES_H_

//For Message Ids
enum{
    Sensor_Data_Id = 101,
    Motor_Speed_Id = 203,
    Geo_Receive_Start_Coordinates = 405//,
    //Bt_Receive_Start_Coordinates = 503
};

//For Speed
enum{
    reverse,
    brake,
    slow,
    medium,
    fast
};

//For Directions
enum{
    straight,
    full_left,
    slight_left,
    full_right,
    slight_right

};

bool receiveSensorValues(void);
bool checkSensorValues(void);

#endif /* RECEIVE_SENSOR_VALUES_H_ */
/*
//For LD Indicators
{
    min_m = 5,
    l = 10,
    m = 20,
    r = 30,
    l_r = 40,
    m_l = 50,
    m_l_r = 60,
    m_r = 70,
    min_l_r = 80,
    no_obstacle = 90,
}
*/
