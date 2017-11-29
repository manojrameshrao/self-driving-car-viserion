#include "bluetoothreceive.h"
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "gpio.hpp"
#include "uart3.hpp"
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "sendoncan.h"
#include <iostream>



using namespace std;



string coordinates;
double latti=0;
double longi=0;
//int size=0;
char *latt;
char *longii;
char temp[21];
static char point[]="POINT";
static char stats[]="STATS";
static char sstat[]="SSTAT";
static char start[]="START";
static char stop[]="VSTOP";


void bluetoothreceiveinit(Uart3& u3)
{

u3.init(9600);
}
bool stat;
char send[]="L37.338623, -121.879734";
bool recieve_cmd_from_bluetooth(Uart3& u3)
{

	if(u3.getRxQueueSize()==5)
	{

		u3.gets(temp,5,0);
		//comm = temp;
	   // printf("%s \n",temp);


		if(strcmp(temp, point) == 0)
		{
			//send can msg to geo asking for coordinates
			u3.putline(send,portMAX_DELAY);
		}

		else if(strcmp(temp,stats) == 0)
		{
			stat=1;
			//send stat data to blutooth in 10hz

		}

		else if(strcmp(temp,sstat) == 0)
		{
			stat=0;
			//stop stat data to blutooth in 10hz
		}


		else if(strcmp(temp,start) == 0)
		{
			//send can msg to master to start car
		}

		else if(strcmp(temp,stop) == 0)
		{
			// send can msg to master to stop
		}
	}

	return stat;
}

void receiveAllCoordinates(Uart3& u3){



	if(u3.getRxQueueSize()>=21)
	{


		u3.gets(temp,21,0);
		{
		coordinates = temp;

		latt=strtok(temp,",");
		longii=strtok(NULL,",");

		latti=strtod(latt,NULL);
		longi=strtod(longii,NULL);


		sendAllCordinates(latti,longi);

		//points.set_lattitude(latti);
		//points.set_longitude(longi);

		//recieve.push_back(points);

		/*if(size < recieve.size())
		{
*/

/*


		}
*/

		printf("lat= %f long= %f \n",latti,longi);
		LE.toggle(1);
		}
	}

}


