#ifndef SPEED_CONTROL_H
#define SPEED_CONTROL_H
#include <QString>

class speed_control
{
 public:

    QString Measure_description;
    std::chrono::system_clock::time_point end;
    std::chrono::system_clock::time_point start;
    speed_control();
    speed_control(QString Description);
    ~speed_control();
    int speed_control_start();

    int speed_control_stop();
};



#endif // SPEED_CONTROL_H
