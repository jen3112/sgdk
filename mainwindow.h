#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "z_thread.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QTime>

#include <QTimer>
#include <QPointer>
#include <QTextEdit>
#include <QTcpSocket>
#include <vector>
#include <string>
#include "qcustomplot.h"
#include "brd_table.h"

#include "start_isa.h"
#include "edit.h"
#include "graph.h"
#include "mainwindowstate.h"

class QAction;  // Просто объявляем класс, это вместо include, но быстрее компилируется

namespace SGDK
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QActionGroup *agModes;
    QAction *actMembraneMode ;
    QAction *act_Cfg1 ;
    QAction *act_Cfg2 ;
    QAction *act_edit_cfg;

    QAction *act_temp_kvant;
    QAction *act_temp_grad;

    QToolBar *tbModes;

    QTimer tmr;

    enum TIMER_MODES
    {
        TMR_NORMAL = 0,
        TMR_MAX_INTERVAL = 1
    };

    int timer_mode = TIMER_MODES::TMR_NORMAL;
    start_isa isa;

    void ch_tmr_mode(TIMER_MODES mode);



private slots:
    void updateTime();  // срабатывает по таймеру
    void onReadyRead();  // данные сокета готовы для чтения
    void fill_property_block(); // Перенос значений из выбранной строки в графу настроек

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void show_train();
    void average_wheels();

    void reconnect_after_pause(DATA::BoardCfg *brd_cfg_pointer);

    void delete_closed_membrane_wins(DATA::BoardCfg *cfg_pointer);

    void fill_dac_cell(QLineEdit* field, int row_id, int col_id);

    // byte_to_send - 0- отправка двух байт, 1 -только первый, 2-только второй
    void send_cmd_to_addr(int addr, QString command_str, int byte_to_send = 0, int cmd = 0xA0);

    void stop_all_threads();
    void stop_all();
    void ch_cfg_mode(int nomb);

    QDockWidget *TableDockWidget;

    QPushButton *start_button;
    QPushButton *send_cmd_button;
    QPushButton *send_meandr_button;
    QPushButton *metrol_button;

    QLineEdit *old_dac0_textblock;
    QLineEdit *old_dac1_textblock;
    QLineEdit *old_dac2_textblock;
    QLineEdit *old_dac3_textblock;
    QLineEdit *brd_nomb_textblock;
    QLineEdit *freq_textblock;
    QLineEdit *temper_limit_textblock;
    QLineEdit *new_temper_limit_textblock;

    QLineEdit *param_textblock;

    QLineEdit *new_dac0_textblock;
    QLineEdit *new_dac1_textblock;
    QLineEdit *new_dac2_textblock;
    QLineEdit *new_dac3_textblock;
    QLineEdit *new_nomb_textblock;
    QLineEdit *new_freq_textblock;

    QLineEdit *command_textblock;

    QStringList Chanel_group_list;

    QLabel *lbl_chanal;

    char send_cmd_buff[4];

    int graph_brd_row_nomb = -1;

    brd_table   *Tab;
    edit   *T_widget;
    QFrame *Frame1;
    QDockWidget *Dw1;
    QDockWidget *Dw2;
    QFrame *dac_frame;
    QVBoxLayout *v_main_graph_layout;
    QFrame *grfph_frame;
    QFrame *dac_title_frame;

    void closeEvent(QCloseEvent* event) override;

    void send_cmd();

protected slots :
    void TestMembrane_slot(void);
    void cfg1_Mode(void);
    void cfg2_Mode(void);
    void start_graph(void);
    void set_brd_param(void);
    void send_cmd_butt_press(void);
    void send_meandr_cmd(void);
    void send_kalibr_cmd(void);
    void send_restart_cmd(void);
    void calibrate_all_cmd(void);
    void autonum_cmd();

    void save_table_cmd();
    void set_temp_grad();
    void set_temp_kvant();

    void chanel_group_changed();
    void edit_app_cfg();

    void send_2kHz_cmd();
    void send_4kHz_cmd();

    void send_short_cmd_butt_press();

};

} //namespace SGDK
#endif // MAINWINDOW_H
