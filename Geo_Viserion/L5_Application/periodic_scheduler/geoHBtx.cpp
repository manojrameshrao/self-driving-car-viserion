/*
 * geoHBtx.cpp
 *
 *  Created on: 22-Oct-2017
 *      Author: Admin
 */

#include "periodic_scheduler/geoHBtx.h"
#include "_can_dbc/generated_Viserion.h"


void geo_heartbeat(void)
{
    HB_GEO_t geo_hb;
    geo_hb.GEO_ALIVE = 1;
    dbc_encode_and_send_HB_GEO(&geo_hb);
}


