#include "graph.h"

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCloseEvent>
#include <QtGui>

#include "save_table_csv.h"
#include "application.h"
#include "int_ring_buf.h"
#include <qcustomplot.h>
#include "time_str.h"
#include "create_dirs.h"
#include "save_table_csv.h"

namespace SGDK
{

/* -------------------------------------------------------------*/

void graph::closeEvent(QCloseEvent* event)
{
    etalon_reader.etalon_thread_state = 0; // приводит к закрытию потока чтения эталона
    if (model->rowCount()>0)
    {
        save_table_last_str_to_log(model);
    }

    stop();

    Sleep(500); // после сброса флага start_graph ожидаю прекращения заполнения буфера

    wGraphic.clearGraphs();   //очищаем все графики

    // очистка буфера
    gr_time.clear();
    for (int n=0; n>5; n++)
    {
        gr_y[n].clear();
    }

    if (graph_file.isOpen())
    {
        graph_file.close();
    }

    event->accept();  //для разрешения закрытие окна

    model->clear();
    delete model;

    v_main_graph_layout->removeWidget(&wGraphic);
    v_main_graph_layout->removeWidget(metrol_frame);

    delete v_main_graph_layout;

    for (int i=0; i<16; i++) dac_title_layout->removeWidget(&lbl[i]);

    delete dac_title_layout;

    dac_layout->removeWidget(dac_title_frame);
    dac_layout->removeWidget(old_dac_frame);
    dac_layout->removeWidget(dac_frame_row[0]);
    dac_layout->removeWidget(dac_frame_row[1]);

    delete new_dac_layout[0];
    delete new_dac_layout[1];

    delete dac_frame_row[0];

    bottom_layout->removeWidget(start_button);
    bottom_layout->removeWidget(dac_frame);

    // сброс флага наличия открытого окна с графиком для данной платы
    graph_win_is_open = false;
}

/* -------------------------------------------------------------*/

void graph::graph_window_init(DATA::BoardCfg* board_pointer)
{
    board_cfg_pointer = board_pointer;

    //brd_nomb = board_cfg_pointer->BrdNomb ;
    QString title = "Board №" + QString::number(board_cfg_pointer->BrdNomb) +
                 ". IP = " + QString::number(board_cfg_pointer->ip[0]) +
                 "." + QString::number(board_cfg_pointer->ip[1]) +
                 "." + QString::number(board_cfg_pointer->ip[2]) +
                 "." + QString::number(board_cfg_pointer->ip[3]) +
                 "    Эталон подключен к COM" + QString::number(APP->pModel->Cfg.etalon_com_port_name);

    setWindowTitle( title ); // заголовок диалога

    // формирую имя временного файла для записи графика   ??????????????
    graph_file_name = "graph_" + QString::number(board_cfg_pointer->row_nomb_in_table)+ "-" + QString::number(board_cfg_pointer->BrdNomb) + ".csv";

    v_main_graph_layout = new QVBoxLayout(this);

    // Добавляю виджет графика
    v_main_graph_layout->addWidget(&wGraphic);
    wGraphic.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    metrol_init();  // ---- меролохия -------

    // нижний фрейм с кнопками
    bottom_frame_init();
    v_main_graph_layout->addWidget(bottom_frame); // добавляем фрейм 2 на основной лэйаут

    wGraphic.clearGraphs();// очищаем все графики

    //Добавляем графики в widget
    for (int n=0; n<18; n++)
    {
        wGraphic.addGraph();
        wGraphic.graph(n)->setPen(QPen( APP->pModel->chanel_color[n].color ));
    }

    // Инициализация таймера обновления графика
    graph_tmr.setInterval(600);

    connect(&graph_tmr, SIGNAL(timeout()), this, SLOT(update_graph_timer()));

}

/* -------------------------------------------------------------*/

graph::graph()
    : QDialog()
{

}


graph::graph(QWidget *parent, DATA::BoardCfg* board_pointer)
    : QDialog(parent)
{
    graph_window_init( board_pointer );
}

/*------------------------------------------------------------------*/

void graph::metrol_init()
{
    metrol_frame = new QFrame(this);
    // стиль фрейма
    metrol_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    metrol_layout = new QHBoxLayout(metrol_frame);
    metrol_layout->setContentsMargins(0,0,0,0);

    {   // кнопки метролуха
        QFrame *metrol_but_frame = new QFrame(this);
        // стиль фрейма
        metrol_but_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
        metrol_but_frame->setMaximumWidth(90);

        metrol_but_layout = new QVBoxLayout(metrol_but_frame);
        { // добавляю кнопки и поле ввода на лэйаут
            metrol_interval_frame = new QFrame(this);
            metrol_interval_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

            metrol_interval_layout = new QHBoxLayout(metrol_interval_frame);

            // Добавляем пустой эл-т чтобы сдвинуть кнопки
            vert_spacer = new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding);
            metrol_but_layout->addItem( vert_spacer );

            {
                QLabel *interval_label = new QLabel("Инт.(сек):");
                interval_izmer_time.setText("0");
                metrol_interval_time = 0;
                connect(&interval_izmer_time, SIGNAL( editingFinished()),this, SLOT(metrol_interval_time_is_changed()));
                metrol_but_layout->addWidget(interval_label);
                metrol_but_layout->addWidget(&interval_izmer_time);
            }
            {
                QLabel *interval_label = new QLabel("Инт.(кг):");
                interval_izmer_weigth.setText("0");
                metrol_interval_weight = 0;
                connect(&interval_izmer_weigth, SIGNAL( editingFinished()),this, SLOT(metrol_interval_weigth_is_changed()));
                metrol_but_layout->addWidget(interval_label);
                metrol_but_layout->addWidget(&interval_izmer_weigth);
            }

            zero_metrol_button =  new QPushButton(this);
            zero_metrol_button->setText("Zero");

            // Добавляем кнопку на лэйаут
            metrol_but_layout->addWidget(zero_metrol_button);
            connect(zero_metrol_button, SIGNAL(clicked()),this, SLOT(metrol_zero()));

            fix_metrol_button =  new QPushButton(this);
            fix_metrol_button->setText("FIX");

            metrol_but_layout->addWidget(fix_metrol_button);
            connect(fix_metrol_button, SIGNAL(clicked()),this, SLOT(metrol_fix()));

            next_metrol_button =  new QPushButton(this);
            next_metrol_button->setText("NEXT");

            metrol_but_layout->addWidget(next_metrol_button);
            connect(next_metrol_button, SIGNAL(clicked()),this, SLOT(metrol_next()));

            auto_metrol_button =  new QPushButton(this);
            auto_metrol_button->setText("AUTO");

            metrol_but_layout->addWidget(auto_metrol_button);
            connect(auto_metrol_button, SIGNAL(clicked()),this, SLOT(metrol_auto()));

            save_metrol_button =  new QPushButton(this);
            save_metrol_button->setText("SAVE");

            // Добавляем кнопку на лэйаут
            metrol_but_layout->addWidget(save_metrol_button);
            connect(save_metrol_button, SIGNAL(clicked()),this, SLOT(metrol_save()));
        }
        metrol_layout->addWidget(metrol_but_frame);
    }

    // табличка метролуха
    model=  new QStandardItemModel(0, 13); // 3 строки, 2 столбца

    init_metrol_model();

    metrol_table.setParent(metrol_frame);
    metrol_table.setModel(model);
    metrol_table.resizeColumnsToContents();
    metrol_layout->addWidget(&metrol_table);
    v_main_graph_layout->addWidget(metrol_frame);

    model->appendRow(new QStandardItem("--"));
}

/*------------------------------------------------------------------*/

void graph::bottom_frame_init()
{
    // вспомогательный Фрейм с кнопками и значениями ЦАП
    bottom_frame = new QFrame(this);
    // стиль фрейма
    bottom_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    bottom_frame->setFixedHeight(80);

    bottom_layout = new QHBoxLayout(bottom_frame);
    bottom_layout->setContentsMargins(0,0,0,0);

    start_button =  new QPushButton(this);
    start_button->setText("Start");

    // Добавляем кнопку на лэйаут
    bottom_layout->addWidget(start_button);
    connect(start_button, SIGNAL(clicked()),this, SLOT(start_graph()));

    // создаю фрейм для значений цап
    dac_frame = new QFrame(this);
    dac_frame->setStyleSheet("background: white");

    //помещаю фрейм с цапами на лэйаут с кнопками
    bottom_layout->addWidget(dac_frame);

    //создаем вертикальный лэйаут для старых значений сверху и новых снизу
    dac_layout = new QVBoxLayout(dac_frame);
    dac_layout->setContentsMargins(0,0,0,0);

    // создаю 2 фрейма для старых и новых значений ЦАП
    dac_title_frame = new QFrame(this);
    dac_frame_row[0] = new QFrame(this);
    dac_frame_row[1] = new QFrame(this);

    //и помещаю их на лэйаут
    dac_layout->addWidget(dac_title_frame);
    dac_layout->addWidget(dac_frame_row[0]);
    dac_layout->addWidget(dac_frame_row[1]);

    //добавляю к ним разметку
    dac_title_layout = new QHBoxLayout(dac_title_frame);
    new_dac_layout[0] = new QHBoxLayout(dac_frame_row[0]);
    new_dac_layout[1] = new QHBoxLayout(dac_frame_row[1]);

    dac_title_layout->setContentsMargins(0,0,0,0);
    new_dac_layout[0]->setContentsMargins(0,0,0,0);
    new_dac_layout[1]->setContentsMargins(0,0,0,0);

    // синий фреим - добавляю галки для вывода графиков
    for(int i=0; i<16; i++)
    {
        {  //шапка
            dac_title_layout->addWidget(&lbl[i]);
            lbl[i].setStyleSheet(APP->pModel->chanel_color[i].color_style);
            lbl[i].setText("D" + QString::number(i));
            lbl[i].setMaximumWidth(22);
        }
        Gr_check_box[i] = new QCheckBox(dac_frame_row[0]);

        Gr_check_box[i]->setChecked(board_cfg_pointer->show_graph[i]);
        Gr_check_box[i]->setMaximumWidth(22);

        new_dac_layout[0]->addWidget(Gr_check_box[i]);
        connect(Gr_check_box[i], SIGNAL(clicked(bool)), this, SLOT(gr_check_box_change()));

        if(i<4)
        {
            Gr_check_box[i]->setChecked(true);
            Gr_check_box[i]->setVisible(true);
            lbl[i].setVisible(true);
        }
        else
        {
            Gr_check_box[i]->setChecked(false);
            Gr_check_box[i]->setVisible(false);
            lbl[i].setVisible(false);
        }
    }
    {
        new_dac_layout[1]->addWidget(&lbl[16]);
        lbl[16].setStyleSheet(APP->pModel->chanel_color[16].color_style);
        lbl[16].setText("Sum");
        lbl[16].setMaximumWidth(25);

        Gr_check_box[16] = new QCheckBox(dac_frame_row[1]);

        Gr_check_box[16]->setChecked(board_cfg_pointer->show_graph[16]);
        Gr_check_box[16]->setMaximumWidth(18);
        Gr_check_box[16]->setMinimumWidth(18);

        new_dac_layout[1]->addWidget(Gr_check_box[16]);
        connect(Gr_check_box[16], SIGNAL(clicked(bool)), this, SLOT(gr_check_box_change()));
    }
    {
        new_dac_layout[1]->addWidget(&lbl[17]);
        lbl[17].setStyleSheet(APP->pModel->chanel_color[17].color_style);
        lbl[17].setText("X-Y");
        lbl[17].setMaximumWidth(20);

        Gr_check_box[17] = new QCheckBox(dac_frame_row[1]);

        Gr_check_box[17]->setChecked(board_cfg_pointer->show_graph[16]);
        Gr_check_box[17]->setMaximumWidth(18);

        new_dac_layout[1]->addWidget(Gr_check_box[17]);
        connect(Gr_check_box[17], SIGNAL(clicked(bool)), this, SLOT(gr_check_box_change()));
    }
    {
        {  //  режим метролуха
            new_dac_layout[1]->addWidget(&lbl[18]);
            //lbl[18].setStyleSheet(APP->pModel->chanel_color[17].color_style);
            lbl[18].setText("МЕТРОЛОГИЯ");
            lbl[18].setMaximumWidth(80);
        }
        Gr_check_box[18] = new QCheckBox(dac_frame_row[1]);

        //Gr_check_box[18]->setChecked(APP->pModel->Cfg.BrdConfVector[graph_board_id].metrol_enabled);
        Gr_check_box[18]->setChecked(false);
        Gr_check_box[18]->setMaximumWidth(18);

        new_dac_layout[1]->addWidget(Gr_check_box[18]);
        connect(Gr_check_box[18], SIGNAL(clicked(bool)), this, SLOT(metr_check_box_change()));
    }

    board_cfg_pointer->metrol_enabled = false;
    metrol_frame->setEnabled(false);
    metrol_frame->setVisible(false);

    // Добавляем пустой эл-т чтобы сдвинуть кнопки
    spacer = new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum);
    bottom_layout->addItem( spacer );
}

/*------------------------------------------------------------------*/

void graph::metrol_interval_time_is_changed()
{
    metrol_interval_time = (interval_izmer_time.text().toInt());
    interval_izmer_time.setText(QString::number(metrol_interval_time));
}

/*------------------------------------------------------------------*/

void graph::metrol_interval_weigth_is_changed()
{
    metrol_interval_weight = (interval_izmer_weigth.text().toInt());
    interval_izmer_weigth.setText(QString::number(metrol_interval_weight));
}

/*------------------------------------------------------------------*/

void graph::init_metrol_model()
{
    // установка заголовков таблицы
    model->setHeaderData(0, Qt::Horizontal, "Time");
    model->setHeaderData(1, Qt::Horizontal, "№ exper");
    model->setHeaderData(2, Qt::Horizontal, "Эталон");
    model->setHeaderData(3, Qt::Horizontal, "D1");
    model->setHeaderData(4, Qt::Horizontal, "D2");
    model->setHeaderData(5, Qt::Horizontal, "D3");
    model->setHeaderData(6, Qt::Horizontal, "D4");
    model->setHeaderData(7, Qt::Horizontal, "SUMM");
    model->setHeaderData(8, Qt::Horizontal, "Temp 1");
    model->setHeaderData(9, Qt::Horizontal, "Temp 2");
    model->setHeaderData(10, Qt::Horizontal, "Temp 3");
    model->setHeaderData(11, Qt::Horizontal, "Temp 4");
    model->setHeaderData(12,Qt::Horizontal, "Delta");
}

/*------------------------------------------------------------------*/

void graph::metr_check_box_change()
{
    board_cfg_pointer->metrol_enabled = Gr_check_box[18]->isChecked();
    metrol_frame->setEnabled(Gr_check_box[18]->isChecked());
    metrol_frame->setVisible(Gr_check_box[18]->isChecked());
}

/*------------------------------------------------------------------*/

void graph::metrol_next()
{

    model->setItem(model->rowCount() - 1,1,new QStandardItem( QString::number(++metrol_nomer_izmeren) ));
}

/*------------------------------------------------------------------*/

void graph::metrol_save()
{
    SGDK::save_table_csv(model);
}

/*------------------------------------------------------------------*/

void graph::metrol_zero()
{
    //сохраняю в лог последнюю строчку
    save_table_last_str_to_log(model);

    model->removeRows(0,model->rowCount());
    model->appendRow(new QStandardItem("--"));
    metrol_nomer_izmeren = 0;

    for (int i=0; i<4; i++)
    {
        board_cfg_pointer->metrol_zero[i] = (board_cfg_pointer->metrol_data[i] +
                                             board_cfg_pointer->metrol_data[i+4] +
                                             board_cfg_pointer->metrol_data[i+8] +
                                             board_cfg_pointer->metrol_data[i+12] )/4;
    }
}

/*------------------------------------------------------------------*/

void graph::metrol_fix()
{
    //сохраняю в лог последнюю строчку
    save_table_last_str_to_log(model);

    //table_model->data(index_of_cell)
    prev_sum = dat_sum;


    model->appendRow(new QStandardItem("--"));
    int last_row = model->rowCount() - 1;
    model->setItem(last_row,1,new QStandardItem( QString::number(metrol_nomer_izmeren) ));
    //model->setItem(last_row,12,new QStandardItem( QString::number(metrol_nomer_izmeren) ));

    metrol_prev_time = QDateTime::currentDateTime();
    metrol_table.scrollToBottom();
}

/*------------------------------------------------------------------*/

void graph::metrol_auto()
{

}

/*------------------------------------------------------------------*/

graph::~graph()
{







}

/*------------------------------------------------------------------*/

void graph::gr_check_box_change()
{
    for (int i=0; i<18; i++)
    {
        board_cfg_pointer->show_graph[i] = Gr_check_box[i]->isChecked();
    }

}

/*------------------------------------------------------------------*/

void graph::start_graph(void)
{
    (board_cfg_pointer->gr_win.graph_is_started) ? stop() : start();
}

/*------------------------------------------------------------------*/

void graph::start()
{
    gr_time.fill(0,gr_buff_len+1);

    for (int n=0; n<17; n++)
    {
        gr_y[n].fill(0,gr_buff_len+1);
    }

    buff_pozition = 0;

    if (!graph_file.isOpen())
    {
        QString DirName = APP->pModel->Cfg.archiv_dir + "graph\\" + get_time_str() ;

        create_dir(DirName);

        DirName += "\\" + graph_file_name;

        graph_file.setFileName(DirName);
        graph_file.open(QIODevice::WriteOnly);
    }

    metrol_prev_time = QDateTime::currentDateTime();

    //чтобы после остановки возобновлялось заполнение таблицы
    metrol_interval_time_is_changed();
    metrol_interval_weigth_is_changed();

    graph_tmr.start(500);

    graph_is_started = true;
    start_button->setText("Stop");
}

/*------------------------------------------------------------------*/

void graph::stop()
{

    graph_tmr.stop();

    graph_is_started = false;

    if (graph_file.isOpen())
    {
        graph_file.close();
    }

    start_button->setText("Start");
}


/*------------------------------------------------------------------*/

void graph::update_graph_timer()
{
    // проверяю не поменялось ли кол-во каналов
    if (board_cfg_pointer->ch_nomb_changed)
    {
        board_cfg_pointer->ch_nomb_changed = 0;

        // обновляю видимость чекбоксов
        if (board_cfg_pointer->ch_nomb == 16)
        {
            for (int i=0; i<16; i++)
            {
                Gr_check_box[i]->setChecked(true);
                Gr_check_box[i]->setVisible(true);
                lbl[i].setVisible(true);
            }
        }
        else
        {
            for (int i=0; i<4; i++)
            {
                Gr_check_box[i]->setChecked(true);
                Gr_check_box[i]->setVisible(true);
                lbl[i].setVisible(true);
            }
            for (int i=4; i<16; i++)
            {
                Gr_check_box[i]->setChecked(false);
                Gr_check_box[i]->setVisible(false);
                lbl[i].setVisible(false);
            }
        }
    }

    int ch_nombers = board_cfg_pointer->ch_nomb;
    if (ch_nombers != 4 && ch_nombers != 16) ch_nombers = 4;

    //Проверили что плата передает данные
    if (board_cfg_pointer->State == SGDK::DATA::Connected ||
        board_cfg_pointer->State == SGDK::DATA::Train_detection)
    {

        //Обновляю табличку метролуха если включен режим
        if (board_cfg_pointer->metrol_enabled)
        {
            dat_sum = 0;
            int last_row = model->rowCount() - 1;
            for (int i=0;i<4;i++)
            {
                int dat = (board_cfg_pointer->metrol_data[i] +
                board_cfg_pointer->metrol_data[4+i] +
                board_cfg_pointer->metrol_data[8+i] +
                board_cfg_pointer->metrol_data[12+i] ) / 4;
                model->setItem(last_row,3+i,new QStandardItem( QString::number(dat) ));
                dat_sum += dat;
            }

            model->setItem(last_row,7,new QStandardItem( QString::number(dat_sum/4) ));

            model->setItem(last_row,12,new QStandardItem( QString::number((dat_sum - prev_sum)/4) ));


            if(APP->pModel->Cfg.temperature_mode == DATA::kvant)
            {
                model->setHeaderData(8, Qt::Horizontal, "T1(Кв)");
                model->setHeaderData(9, Qt::Horizontal, "T2(Кв)");
                model->setHeaderData(10, Qt::Horizontal, "T3(Кв)");
                model->setHeaderData(11, Qt::Horizontal, "T4(Кв)");
                for (int i=0; i<4; i++)
                {
                    //Temp = APP->pModel->Cfg.BrdConfVector[graph_brd_id].Temp_kvanty[i];
                    model->setItem(last_row,8+i,new QStandardItem( QString::number(board_cfg_pointer->Temp_kvanty[i]) ));
                }
            }
            else
            {
                model->setHeaderData(8, Qt::Horizontal, "T1(°C)");
                model->setHeaderData(9, Qt::Horizontal, "T2(°C)");
                model->setHeaderData(10, Qt::Horizontal, "T3(°C)");
                model->setHeaderData(11, Qt::Horizontal, "T4(°C)");
                for (int i=0; i<4; i++)
                {
                    //Temp = (float)APP->pModel->Cfg.BrdConfVector[graph_brd_id].Temp_grad[i]/10;
                    model->setItem(last_row,8+i,new QStandardItem( QString::number((float)board_cfg_pointer->Temp_grad[i]/10) ));
                }
            }


            for (int i=0; i<4; i++)
            {
                //int Temp;
                if(APP->pModel->Cfg.temperature_mode == DATA::kvant)
                    //Temp = APP->pModel->Cfg.BrdConfVector[graph_brd_id].Temp_kvanty[i];
                    model->setItem(last_row,8+i,new QStandardItem( QString::number(board_cfg_pointer->Temp_kvanty[i]) ));
                else
                    //Temp = (float)APP->pModel->Cfg.BrdConfVector[graph_brd_id].Temp_grad[i]/10;
                    model->setItem(last_row,8+i,new QStandardItem( QString::number((float)board_cfg_pointer->Temp_grad[i]/10) ));
            }

            model->setItem(last_row,1,new QStandardItem( QString::number(metrol_nomer_izmeren) ));

            QDateTime t = QDateTime::currentDateTime();
            model->setItem(last_row,0,new QStandardItem( t.toString("hh:mm:ss dd-MM-yyyy") ));



            // чтение эталонного датчика
            weigth_from_etalon = etalon_reader.etalon_weigth;
            //etalon_reader.etalon_weigth = 555;
            model->setItem(last_row,2,new QStandardItem( QString::number(weigth_from_etalon) ));


            // если установлен интервал, то проверяю его
            if (metrol_interval_time>0)
            {
                qint64 seconds = metrol_prev_time.secsTo(t);
                if (seconds > (qint64)(metrol_interval_time))
                {
                    metrol_prev_time = t;
                    metrol_fix();
                }
            }

            metrol_table.resizeColumnsToContents();
        }

        if (board_cfg_pointer->show_graph[17])
        {  // график X-Y
            wGraphic.graph(0)->setData(gr_y[1],gr_y[0]);
            wGraphic.graph(1)->setData(null_vector,null_vector);
            wGraphic.graph(2)->setData(null_vector,null_vector);
            wGraphic.graph(3)->setData(null_vector,null_vector);
            wGraphic.graph(4)->setData(null_vector,null_vector);
        }
        else
        {
            for(int n=0; n<ch_nombers; n++)
            {
                if (board_cfg_pointer->show_graph[n])
                {
                    wGraphic.graph(n)->setData(gr_time,gr_y[n]);
                }
                else
                {
                    wGraphic.graph(n)->setData(null_vector,null_vector);
                }
            }
            for(int n=ch_nombers; n<18; n++)
            {
                // Обнуляю неиспользуемые каналы
                wGraphic.graph(n)->setData(null_vector,null_vector);
            }

        }

        if (board_cfg_pointer->show_graph[16])
        {  // график суммы
            wGraphic.graph(16)->setData(gr_time,gr_y[16]);
        }


        //Для показа границ по оси Oy надо
        //вычислить мин. и макс. значение в векторах
        maxY = 0;
        minY = 100000;
        a = 100000;
        b= 0;

        long int start_time = 1;

        for (long int i=1; i<buff_pozition-1; i++)
        {
            if ( (gr_time[i] == 0) && (gr_y[0][i] == 0) && (gr_y[1][i] ==0) &&  (gr_y[2][i] ==0))  // выбрасываю стартовые точки
            {
                start_time = i;
            }
            else
            {
                // нахожу мин и макс
                for(int j=0; j<18; j++)
                {
                    if (board_cfg_pointer->show_graph[j])
                    {
                        if (gr_y[j][i] < minY) minY = gr_y[j][i];
                        if (gr_y[j][i] > maxY) maxY = gr_y[j][i];
                    }
                }
            }
        }

        if (!board_cfg_pointer->show_graph[17])
        {
            if(buff_pozition>1)   //
            {
                b=gr_time[buff_pozition-1]+1;
            }
            else b=0;
            a=gr_time[start_time]-1;
            if (b<a)
            {
                a=0;
            }
        }

        //Установим область, которая будет показываться на графике
        wGraphic.xAxis->setRange(a, b);//Для оси Ox
        wGraphic.yAxis->setRange(minY-300, maxY+300);//Для оси Oy

        wGraphic.axisRect()->setupFullAxesBox(true);
        wGraphic.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        //И перерисуем график на нашем widget
        wGraphic.replot(QCustomPlot::rpQueuedReplot);
    }

    // если не запущен то запускаю поток чтения эталона
    if (etalon_reader.etalon_thread_state == 0)
    {
        etalon_reader.start();
    }
}

/*------------------------------------------------------------------*/

void graph::offset(int offset_len)
{
    minY = gr_y[0][0];
    maxY = gr_y[0][0];

    for (long int i=offset_len; i<gr_buff_len+1; i++)
    {
        gr_time[i-offset_len] = gr_time[i];
        for (int j=0; j<17; j++)
        {
            gr_y[j][i-offset_len] = gr_y[j][i];
        }
    }
}

/*------------------------------------------------------------------*/

} // namespace SGDK
