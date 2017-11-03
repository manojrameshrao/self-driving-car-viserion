/*
 * trigger_left_and_right.h
 *
 *  Created on: Nov 2, 2017
 *      Author: Miroslav Grubic
 */

#ifndef TRIGGER_LEFT_AND_RIGHT_H_
#define TRIGGER_LEFT_AND_RIGHT_H_

#include "gpio.hpp"


void trigger_left_and_right(uint32_t const &count, GPIO &l_trigger, GPIO &r_trigger);


#endif /* TRIGGER_LEFT_AND_RIGHT_H_ */
