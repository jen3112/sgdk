#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QTextEdit>
#include <QPointer>
#include <vector>

#include "boardsmodel.h"
#include "test_membrane.h"
#include "graph.h"

#define ver 1
#define subver 1


namespace SGDK
{



struct wheel_rez_data
{
    DATA::BoardCfg *wheel_rez_data_brd_cfg_pointer;
    int van_nomb;
    int wheel_nomb;
    double weigth_r;
    int measurement_nomb_r;
    double weigth_l;
    int measurement_nomb_l;
};

class application : public QApplication
{
    Q_OBJECT

  public:

    int prog_version_number = ver;
    int prog_subvers_number = subver;

    BoardModel *pModel;

    void save_config();

  public:

    std::vector <wheel_rez_data> wheels_buff;
    std::vector <wheel_rez_data> wheels_all;
    std::map <int, wheel_rez_data> wheels_average;

    application(int argc, char* argv[]);
    virtual ~application();

    int cfg_nomb = 1;   // № конфиг файла
    void change_cfg();
    void set_cfg();
    QString cfg_file_name();

    QPointer<QTextEdit> logTextEdit;

    BoardModel *model() {return pModel;}

};

} //namespace SGDK

#define APP dynamic_cast< ::SGDK::application*>(qApp)


#endif // APPLICATION_H
