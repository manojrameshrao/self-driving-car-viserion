/*
 * Coordinates.cpp
 *
 *  Created on: Nov 3, 2017
 *      Author: dheem
 */

#include <periodic_scheduler/Coordinates.h>

Coordinates::Coordinates() {
	// TODO Auto-generated constructor stub

}

Coordinates::~Coordinates() {
	// TODO Auto-generated destructor stub
}


double Coordinates::get_lattitude()const
{
	return lattitude;
}

double Coordinates::get_longitude()const
{
	return longitude;
}

void Coordinates::set_lattitude(double value)
{
	lattitude=value;
}
void Coordinates::set_longitude(double value)
{
	longitude=value;
}

