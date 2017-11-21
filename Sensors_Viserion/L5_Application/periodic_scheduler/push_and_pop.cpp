/*
 * push_and_pop.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: Miroslav Grubic
 */


#include "push_and_pop.h"


void push_and_pop(std::list<int>& l_list, std::list<int>& m_list, std::list<int>& r_list, int &l, int &m, int &r){

    static const int max_list_size = 3;

    l_list.push_front(l);
    m_list.push_front(m);
    r_list.push_front(r);

    if(l_list.size() > max_list_size){

        l_list.pop_back();
        m_list.pop_back();
        r_list.pop_back();
    }
}

