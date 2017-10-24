/*
 * receive_heartbeats.hpp
 *
 *  Created on: Oct 23, 2017
 *      Author: Aakash
 */

#ifndef RECEIVE_HEARTBEATS_H_
#define RECEIVE_HEARTBEATS_H_
#include "can.h"


enum HB_t
{
    SENSORS_HB_en = 95,
    MOTOR_HB_en = 96,
    GEO_HB_en = 97,
    BT_HB_en = 98
};

bool receive_heartbeats(can_t canbus,uint32_t mia_inc_time);




#endif /* RECEIVE_HEARTBEATS_H_ */
