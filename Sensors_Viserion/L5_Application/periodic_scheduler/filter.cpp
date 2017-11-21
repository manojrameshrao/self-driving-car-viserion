/*
 * filter.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: Miroslav Grubic
 */

#include "filter.h"
#include "stdio.h"


int filter(std::list<int>& list){

    int sum = 0;
    int maxValue = 0, maxCount = 0;

    for(std::list<int>::iterator i=list.begin(); i != list.end(); ++i) {

        int count = 0;
        sum += *i;
//        printf("%d,", *i);

        for(std::list<int>::iterator j=list.begin(); j != list.end(); ++j) {

            if (*j == *i)
            ++count;
         }

         if(count > maxCount) {

            maxCount = count;
            maxValue = *i;
         }
      }

      if(maxCount == 1){

          int avg = sum / list.size();
//          printf("odd \n");

          return avg;
      }

      return maxValue;
}

