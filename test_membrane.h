#ifndef TEST_MEMBRANE_H
#define TEST_MEMBRANE_H

#include "obrabotka_membrane.h"
#include "obrabotka.h"

#include <QDialog>
#include <QFrame>
#include <qcustomplot.h>
#include <QDebug>
#include <QTimer>
#include <fstream>

#include "data.h"


namespace SGDK
{

class test_membrane : public QDialog
{
    Q_OBJECT

public:
    test_membrane(QWidget *parent, DATA::BoardCfg *brd_cfg_pointer);

    bool ready_to_delete = false;

    QString graph_file_name;
    QFile graph_file ;


    int buff_pozition = 0;

    int gr_buff_len = 40000;
    void offset(int offset_len);

    QVector<double> gr_y, gr_temp, gr_x; // координаты точек

    QVector<double> null_vector;

    double minY;
    double maxY;

    void start();

    void stop();

    obrabotka_membrane membr_obrabotka; // статический объект

    QPushButton *start_button;

    QLineEdit *p_textblock;
    QLineEdit *h_textblock;
    QLineEdit *prev_div_textblock;
    QLineEdit *prev_divdiv_textblock;
    QLineEdit *div_textblock;
    QLineEdit *divdiv_textblock;

    QLineEdit *rez_textblock;

    //int membr_brd_id = -1;
    //int membr_board_row_nomb = -1;


    DATA::BoardCfg* board_cfg_pointer;

    QTimer *tmr;

    int timer_phase = 0;

    QCheckBox *Gr_check_box[6];


    double a,b;

    virtual ~test_membrane();

    int window_is_opened = 1;  // сигнал что пора удалять объект

    QCustomPlot *wGraphic;      // Объявляем объект QCustomPlot

    void closeEvent(QCloseEvent* event) override;

public slots :
    void start_graph(void);
    void update_timer();  // таймер обновления графика
    void gr_check_box_change();
    //void gr1_check_box_change();
    //void gr2_check_box_change();
    //void gr3_check_box_change();
    //void gr4_check_box_change();
};

}  // namespace SGDK

#endif // ITEMEDITDIALOG_H
