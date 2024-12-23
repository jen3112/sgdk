#include <QDateTime>


#include "time_str.h"

namespace SGDK
{

QString  get_time_str()
{
    QDateTime Now = QDateTime::currentDateTime();
    QString time_str = Now.toString();
    time_str.replace(" ","_");
    time_str.replace(":","-");
    return time_str;
}


}   // namespace SGDK
