/*

 * test.cpp
 *
 *  Created on: 20-Nov-2017
 *      Author: Admin



#include "tasks.hpp"
#include "printf_lib.h"

char buffer[100]={0};
char start=0;
int num=0;

bool test::init(void)
{
    u2.init(115200);
    u0_dbg_printf("\nInit\n");
    return true;
}

bool test::run(void *p)
{

    //u2.getChar(&start,0);

   // if(start=='$'){

    for(int i=0;i<70;i++)
    {
        u2.getChar(&buffer[i],0);
    }

    u0_dbg_printf("%d-\n%s",num,buffer);
    num++;
 //   }
    return true;

}


*/
