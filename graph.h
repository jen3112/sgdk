#ifndef GRAPH_H_
#define GRAPH_H_



#include <QDialog>
#include <QFrame>
#include <qcustomplot.h>
#include <QDebug>
#include <QTableView>
#include <QTimer>
#include <fstream>

//#include "data.h"

#include "etalon_thread.h"

namespace SGDK
{

namespace DATA
{
    class BoardCfg;
}


class graph : public QDialog
{
    Q_OBJECT

public:

    //graph(QWidget *parent=0, int board_id=0);
    graph(QWidget *parent, DATA::BoardCfg* board_pointer);

    graph() ;

    // для построения графиков
    bool graph_win_is_open = false;
    //int graph_status = 0;  // включен показ графика или нет
    bool graph_is_started = false;

    int dat_sum = 0;
    int prev_sum = 0;

    etalon_thread etalon_reader;

    int weigth_from_etalon = 0;

    QString graph_file_name;
    QFile graph_file ;

    int buff_pozition = 0;

    int metrol_nomer_izmeren = 0;

    int gr_buff_len = 10000;
    void offset(int offset_len);

    QVector<double> gr_y[18], gr_temp, gr_time; // координаты точек

    QVector<double> gr_summ;

    QVector<double> null_vector;

    double minY;
    double maxY;

    void start();

    void stop();

    QLabel lbl[19];

    QVBoxLayout *dac_layout;

    QHBoxLayout *dac_title_layout;
    QHBoxLayout *old_dac_layout;
    QHBoxLayout *new_dac_layout[2];
    QVBoxLayout *metrol_but_layout;

    QFrame *metrol_frame;

    QFrame *dac_frame;
    QHBoxLayout *bottom_layout;

    QHBoxLayout *metrol_layout;

    QFrame *dac_title_frame;

    QFrame *old_dac_frame;
    QFrame *dac_frame_row[2];

    QSpacerItem *spacer;
    QSpacerItem *vert_spacer;

    QPushButton *start_button;
    QPushButton *zero_metrol_button;
    QPushButton *fix_metrol_button;
    QPushButton *auto_metrol_button;
    QPushButton *save_metrol_button;
    QPushButton *next_metrol_button;


    QTimer graph_tmr;

    int graph_timer_phase = 0;

    QCheckBox *Gr_check_box[19];

    double a,b;

    virtual ~graph();

    int window_is_opened = 1;  // сигнал что пора удалять объект

    QCustomPlot wGraphic;      // Объявляем объект QCustomPlot

    QTableView metrol_table;

    QStandardItemModel* model;

    QVBoxLayout *v_main_graph_layout;
    QHBoxLayout *metrol_interval_layout;
    QFrame *metrol_interval_frame;
    QLineEdit interval_izmer_time;
    QLineEdit interval_izmer_weigth;

    QFrame *bottom_frame;
    //int brd_nomb;
    //int graph_board_id;
    DATA::BoardCfg* board_cfg_pointer;


    void closeEvent(QCloseEvent* event) override;
    void init_metrol_model();
    void metrol_init();
    void bottom_frame_init();
    void graph_window_init(DATA::BoardCfg* board_pointer);

    int metrol_interval_time = 0;
    int metrol_interval_weight = 0;

    QDateTime metrol_prev_time;
    int metrol_prev_weight = 0;

public slots :
    void start_graph(void);
    void update_graph_timer();  // таймер обновления графика
    void gr_check_box_change();
    void metrol_zero();
    void metrol_fix();
    void metrol_auto();
    void metrol_next();
    void metrol_save();
    void metr_check_box_change();
    void metrol_interval_time_is_changed();
    void metrol_interval_weigth_is_changed();


};

}  // namespace SGDK

#endif // ITEMEDITDIALOG_H
