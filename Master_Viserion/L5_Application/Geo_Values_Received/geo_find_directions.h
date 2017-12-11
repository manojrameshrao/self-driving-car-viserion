/*
 * geo_find_directions.h
 *
 *  Created on: 10-Dec-2017
 *      Author: Admin
 */

#ifndef GEO_FIND_DIRECTIONS_H_
#define GEO_FIND_DIRECTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Sensor_Values_Received/receive_sensor_values.h"

//bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx, unsigned int speed, unsigned int direction);
//Do we need arguments?? extern variables, can put them directly into speed and direction
//bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx); //Trial : Everything extern
bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx, unsigned int& pSpeed, unsigned int& pDirection); //Trial : Parameter Passing required

#ifdef __cplusplus
}
#endif

#endif /* GEO_FIND_DIRECTIONS_H_ */
