#include "bluetoothtransmission.h"
#include "uart3.hpp"

#include <stdio.h>


void sendtoapp(char *data,Uart3 &u3)
{
	//puts(data);
	u3.putline(data,portMAX_DELAY);
}
