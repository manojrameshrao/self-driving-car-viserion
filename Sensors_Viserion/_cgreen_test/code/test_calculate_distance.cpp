#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen;

// The file under test
#include "calculate_distance.h"

// Files we need to include
#include "get_adc.h"

int get_adc(int channel_number){ return mock(channel_number); }

Ensure(value_is_greater_then_five)
{  
	int l = 0, r = 0, m = 0;
	
	expect(get_adc, will_return(70));
	expect(get_adc, will_return(1000));
	expect(get_adc, will_return(3200));
	
	calculate_distance(l, r, m);
	
	assert_that(l, is_greater_than(5));
	assert_that(l, is_less_than(80));
	
	assert_that(r, is_greater_than(5));
	assert_that(r, is_less_than(80));
	
	assert_that(m, is_greater_than(5));
	assert_that(m, is_less_than(255));
}

TestSuite *calculate_distance_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, value_is_greater_then_five);

    return suite;
}