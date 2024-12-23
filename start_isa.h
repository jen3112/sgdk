#ifndef START_ISA_H
#define START_ISA_H

#include <QThread>
#include <QString>
#include "z_thread.h"

namespace SGDK
{
class start_isa : public QThread
{
public:
    start_isa();

    static bool active;  // проверка нет ли уже запущенного экземпляра

    //std::map<DATA::BoardCfg *, Z_thread *> *z_boards_map_pointer;  // сюда надо передать указатель на z_board_map
    QString get_filename_to_copy_isaev(QString path, int weight_prog_num, int weight_num);
    void run();
};

}  //namespace SGDK

#endif // START_ISA_H
