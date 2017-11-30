#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen\cgreen.h>
#include <cgreen\mocks.h>

using namespace cgreen;

//file under test
#include "L5_Application/Sensor_Values_Received/receive_sensor_values.h"


extern "C" int sensorValueLeft(void)
{
    return mock();
}
extern "C" int sensorValueRight(void)
{
    return mock();
}
extern "C" int sensorValueMiddle(void)
{
    return mock();
}

Ensure(detect_middle_and_stop)
{
    // less than 10
    expect(sensorValueLeft,will_return(25));
    expect(sensorValueRight,will_return(10));
    expect(sensorValueMiddle,will_return(8));
    assert_equal(checkSensorValues(),1); //brake
}
Ensure(middle_detected_but_far away)
{
    expect(sensorValueLeft,will_return(15));
    expect(sensorValueRight,will_return(30));
    expect(sensorValueMiddle,will_return(15));
    assert_equal(checkSensorValues(),2); //go slow and straight
}
Ensure(all_sensors_detected)
{
    expect(sensorValueLeft,will_return(15));
    expect(sensorValueRight,will_return(15));
    expect(sensorValueMiddle,will_return(15));
    assert_equal(checkSensorValues(),1);  //brake
}
Ensure(left_right_detected)
{
    expect(sensorValueLeft,will_return(20));
    expect(sensorValueRight,will_return(100));
    expect(sensorValueMiddle,will_return(20));
    assert_equal(checkSensorValues(),2); //go slow and straight
}
Ensure(left_middle_detected)
{
    expect(sensorValueLeft,will_return(1));
    expect(sensorValueRight,will_return(1));
    expect(sensorValueMiddle,will_return(1));
    assert_equal(checkSensorValues(),3); //go fast to right
}
Ensure(right_middle_detected)
{
    expect(sensorValueLeft,will_return(1));
    expect(sensorValueRight,will_return(1));
    expect(sensorValueMiddle,will_return(1));
    assert_equal(checkSensorValues(),4); //go fast to left
}
Ensure(only_left_detected)
{
    expect(sensorValueLeft,will_return(10));
    expect(sensorValueRight,will_return(35));
    expect(sensorValueMiddle,will_return(45));
    assert_equal(checkSensorValues(),5); //take slight right
}
Ensure(only_right_detected)
{
    expect(sensorValueLeft,will_return(35)));
    expect(sensorValueRight,will_return(14));
    expect(sensorValueMiddle,will_return(36));
    assert_equal(checkSensorValues(),6); //take slight left
}
TestSuite *motor_driver_get_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, detect_middle_and_stop);
    add_test(suite, middle_detected_but_far_away);
    add_test(suite, all_sensors_detected);
    add_test(suite, left_right_detected);
    add_test(suite, left_middle_detected);
    add_test(suite, right_middle_detected);
    add_test(suite, only_left_detected);
    add_test(suite, only_right_detected);
    return suite;
}
