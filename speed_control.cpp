#include <iostream>
#include <chrono>
#include <ctime>
#include "speed_control.h"

speed_control::speed_control()
{
    speed_control("Unknown");
}
speed_control::speed_control(QString Description)
{
    Measure_description = Description;
    speed_control_start();
}
speed_control::~speed_control()
{
    speed_control_stop();
}
int speed_control::speed_control_start()
{
    start = std::chrono::system_clock::now();
    // Some computation here

}
int speed_control::speed_control_stop()
{
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout //<< "finished computation at " << std::ctime(&end_time)
              << "Proc: " << Measure_description.toStdString() << " time: " << elapsed_seconds.count() << "s"
              << std::endl;
}
