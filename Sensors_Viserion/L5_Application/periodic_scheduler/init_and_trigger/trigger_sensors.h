/*
 * trigger_sensors.h
 *
 *  Created on: Nov 17, 2017
 *      Author: Miroslav Grubic
 */

#ifndef TRIGGER_SENSORS_H_
#define TRIGGER_SENSORS_H_

#include "gpio.hpp"


void trigger_sensors(uint32_t const &count, GPIO &l_trigger, GPIO &m_trigger, GPIO &r_trigger, GPIO &b_trigger);


#endif /* TRIGGER_SENSORS_H_ */
