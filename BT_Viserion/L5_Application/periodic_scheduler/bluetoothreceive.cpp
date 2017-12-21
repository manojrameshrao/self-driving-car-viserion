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
#include "_can_dbc/generated_Viserion.h"

GET_START_COORDINATES_t get ={0};
SEND_NO_OF_CHECKPOINTS_t check_num={0};

GPIO errpin (P2_6);


using namespace std;


int points_count = 0;
int app_count = 0;
int wait_count=0;
bool count_check=0;
string coordinates;
double latti=0;
double longi=0;
//int size=0;
char app[3];
char *latt;
char *longii;
char temp[21];
static char point[]="POINT";
static char stats[]="STATS";
static char sstat[]="SSTAT";
static char start[]="START";
static char stop[]= "CSTOP";
static char cstop[]="CENDS";
static char count[]="COUNT";


void bluetoothreceiveinit(Uart3& u3)
{
u3.init(9600);
errpin.setAsOutput();
}
bool stat;
char ssend[]="L37.338623,-121.879734";
bool recieve_cmd_from_bluetooth(Uart3& u3)
{
	if (count_check == 1 )
	{
		//printf("%d \n", u3.getRxQueueSize());

		u3.gets(app,3,0);
	//printf("app sent = %s \n",app);
		{

		app_count=atoi(app);

		if(app_count == points_count)
		{
				//printf("Count = %d They match ",points_count);
				check_num.BT_NO_OF_COR=points_count;
				dbc_encode_and_send_SEND_NO_OF_CHECKPOINTS(&check_num);
				count_check=0;
				points_count=0;
				u3.flush();
				wait_count=0;
				//LE.off(4);

				errpin.setLow();
		}
		else
		{
			wait_count++;
			if(wait_count>50)
			{
				errpin.setHigh();
				//LE.on(4);
			}
		}



		}

	}

	//printf("Que size = %d \n",u3.getRxQueueSize());
	if(u3.getRxQueueSize()==5)
	{

		u3.gets(temp,5,0);
		//comm = temp;
	   // printf("%s \n",temp);


		if(strcmp(temp,stop) == 0)
		{
			// send can msg to master to stop
			sendCarStop();
		}



		else if(strcmp(temp,cstop) == 0)
				{
			printf("End");
			sendEndOfCoordinares();
			count_check=1;
					//send stat data to blutooth in 10hz

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
			sendCarStop();
			sendCarStart();

		}

		else if(strcmp(temp, point) == 0)
				{				//send can msg to geo asking for coordinates

		//	printf("HERE");
		//	u3.putline(ssend);
					get.GET_COORDINATES=111;
					dbc_encode_and_send_GET_START_COORDINATES(&get);


				}

		else if(strcmp(temp,count) == 0)
			{



			}


	}



	return stat;
}

void receiveAllCoordinates(Uart3& u3){



	if(u3.getRxQueueSize()>=21)
	{

		//printf("%d \n",u3.getRxQueueSize());
		u3.gets(temp,21,0);
		{
		points_count++;
		coordinates = temp;
		//printf("%s \n",temp);
		latt=strtok(temp,",");
		longii=strtok(NULL,",");

		latti=strtod(latt,NULL);
		longi=strtod(longii,NULL);

	printf("Lat = %f Long = %f \n ",latti,longi);

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

		//printf("lat= %f long= %f \n",latti,longi);
		LE.toggle(1);
		}
	}

}


