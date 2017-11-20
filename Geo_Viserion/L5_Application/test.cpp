#include "tasks.hpp"
#include "utilities.h"
#include "uart0.hpp"



char buffer[100];
bool test::run(void *p)
{
    for(int i=0;i<70;i++)
    {
        u0.getChar(buffer[i], 0);
    }

    u0_dbg_printf("\n%s",buffer);

   return true;

}
