/*
 * led_on.cpp
 *
 *  Created on: Nov 11, 2017
 *      Author: Miroslav Grubic
 */


#include "led_on.h"
#include "io.hpp"

/**
 * Turn LED 1 if there is obstacle less or equal then 25in from the left sensor
 * Turn LED 2 if there is obstacle less or equal then 25in from the right sensor
 * Turn LED 3 if there is obstacle less or equal then 25in from the middle sensor
 * @l_distance, r_distance, m_distance calculated distances
 */
void led_on(int &l_distance, int &r_distance, int &m_distance){

    if(l_distance <= 25)
        LE.on(1);
    else
        LE.off(1);

    if(m_distance <= 25)
        LE.on(2);
    else
        LE.off(2);

    if(r_distance <= 25)
        LE.on(3);
    else
        LE.off(3);


    /**
     * this is to be used only if want additional indicator
     * of presence of the obstacle from 25 to 40 inches
     */
    #if 0
        if(l_distance <= 40 && l_distance > 25)
            LE.toggle(1);
        else if(l_distance <= 25)
            LE.on(1);
        else
            LE.off(1);

        if(m_distance <= 40 && m_distance > 25)
            LE.toggle(2);
        else if(m_distance <= 25)
            LE.on(2);
        else
            LE.off(2);

        if(r_distance <= 40 && r_distance > 25)
            LE.toggle(3);
        else if(r_distance <= 25)
            LE.on(3);
        else
            LE.off(3);
    #endif
}

