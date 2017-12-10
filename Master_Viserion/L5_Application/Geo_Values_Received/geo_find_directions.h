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

bool take_decision(can_msg_t *crx,dbc_msg_hdr_t *rx);


#ifdef __cplusplus
}
#endif

#endif /* GEO_FIND_DIRECTIONS_H_ */
