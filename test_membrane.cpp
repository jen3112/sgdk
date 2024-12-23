#include "test_membrane.h"
#include "obrabotka_membrane.h"

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCloseEvent>

#include "application.h"
#include "int_ring_buf.h"
#include <qcustomplot.h>

namespace SGDK
{

/* -------------------------------------------------------------*/

void test_membrane::closeEvent(QCloseEvent* event)
{
    stop();
    Sleep(500);

    board_cfg_pointer->start_membrane_graph = 0;
    Sleep(500); // после сброса флага start_graph ожидаю прекращения заполнения буфера

    tmr->stop();
    delete tmr;

    wGraphic->clearGraphs();   //очищаем все графики

    //APP->membrane_wins.remove(board_cfg_pointer);

    // очистка буфера
    gr_x.clear();
    gr_y.clear();


    if (graph_file.isOpen())
    {
        graph_file.close();
    }

    // сброс флага наличия открытого окна с графиком для данной платы
    board_cfg_pointer->membrane_mode_started = 0;

    ready_to_delete = true;

    event->accept();  //для разрешения закрытие окна
}

/* -------------------------------------------------------------*/

test_membrane::test_membrane(QWidget *parent, DATA::BoardCfg *brd_cfg_pointer)
    : QDialog(parent)
{
    board_cfg_pointer = brd_cfg_pointer;

    //int brd_nomb = brd_cfg_pointer->BrdNomb ;
    QString title = "Board №" + QString::number(brd_cfg_pointer->BrdNomb) +
                 ". IP = " + QString::number(brd_cfg_pointer->ip[0]) +
                 "." + QString::number(brd_cfg_pointer->ip[1]) +
                 "." + QString::number(brd_cfg_pointer->ip[2]) +
                 "." + QString::number(brd_cfg_pointer->ip[3]);



    setWindowTitle( title ); // заголовок диалога

    // Инициализация дочернего объекта obrabotka_membrane
    membr_obrabotka.par = this;

    QVBoxLayout *v_main_graph_layout = new QVBoxLayout(this);

    // формирую имя временного файла для записи графика
    graph_file_name = "graph_" + QString::number(brd_cfg_pointer->row_nomb_in_table)+ "-" + QString::number(brd_cfg_pointer->BrdNomb) + ".csv";

    // Добавляю виджет графика
    wGraphic = new QCustomPlot();
    v_main_graph_layout->addWidget(wGraphic);

    // вспомогательный Фрейм с кнопками и значениями ЦАП
    QFrame *bottom_frame = new QFrame(this);
    // стиль фрейма
    //bottom_frame->setStyleSheet("background: red");
    //F2->setFrameStyle(StyledPanel | Sunken);
    bottom_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

    bottom_frame->setFixedHeight(60);

    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_frame);
    bottom_layout->setContentsMargins(0,0,0,0);

    start_button =  new QPushButton(this);
    start_button->setText("Start");

    // Добавляем кнопку на лэйаут
    bottom_layout->addWidget(start_button);
    connect(start_button, SIGNAL(clicked()),this, SLOT(start_graph()));

    // создаю фрейм для значений цап
    QFrame *dac_frame = new QFrame(this);
    dac_frame->setStyleSheet("background: white");

    //помещаю фрейм с производными на лэйаут с кнопками
    bottom_layout->addWidget(dac_frame);

    //создаем вертикальный лэйаут для старых значений сверху и новых снизу
    QVBoxLayout *dac_layout = new QVBoxLayout(dac_frame);
    dac_layout->setContentsMargins(0,0,0,0);

    // создаю 2 фрейма для старых и новых значений ЦАП
    QFrame *dac_title_frame = new QFrame(this);
    QFrame *old_dac_frame = new QFrame(this);
    QFrame *new_dac_frame = new QFrame(this);
    old_dac_frame->setStyleSheet("background: yellow");
    //new_dac_frame->setStyleSheet("background: blue");

    //и помещаю их на лэйаут
    dac_layout->addWidget(dac_title_frame);
    dac_layout->addWidget(old_dac_frame);
    dac_layout->addWidget(new_dac_frame);
    //добавляю к ним разметку
    QHBoxLayout *dac_title_layout = new QHBoxLayout(dac_title_frame);
    QHBoxLayout *old_dac_layout = new QHBoxLayout(old_dac_frame);
    QHBoxLayout *new_dac_layout = new QHBoxLayout(new_dac_frame);
    //dac_title_layout->setMargin(0);
    dac_title_layout->setContentsMargins(0,0,0,0);
    old_dac_layout->setContentsMargins(0,0,0,0);
    new_dac_layout->setContentsMargins(0,0,0,0);

    //шапка
    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("Давление");
        dac_title_layout->addWidget(lbl);
        lbl->setStyleSheet("QLabel { color : blue; }");
    }
    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("Высота");
        dac_title_layout->addWidget(lbl);
        lbl->setStyleSheet("QLabel { color : red; }");
    }
    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("Пред произв");
        dac_title_layout->addWidget(lbl);
        lbl->setStyleSheet("QLabel { color : darkYellow; }");
    }
    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("Пред 2-я произв");
        dac_title_layout->addWidget(lbl);
        lbl->setStyleSheet("QLabel { color : green; }");
    }

    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("Произв");
        dac_title_layout->addWidget(lbl);
    }

    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("2-я произв");
        dac_title_layout->addWidget(lbl);
    }

    //заполняю поля цап значениями
    p_textblock = new QLineEdit(old_dac_frame);
    h_textblock = new QLineEdit(old_dac_frame);
    prev_div_textblock = new QLineEdit(old_dac_frame);
    prev_divdiv_textblock = new QLineEdit(old_dac_frame);
    div_textblock = new QLineEdit(old_dac_frame);
    divdiv_textblock = new QLineEdit(old_dac_frame);

    old_dac_layout->addWidget(p_textblock);
    old_dac_layout->addWidget(h_textblock);
    old_dac_layout->addWidget(prev_div_textblock);
    old_dac_layout->addWidget(prev_divdiv_textblock);
    old_dac_layout->addWidget(div_textblock);
    old_dac_layout->addWidget(divdiv_textblock);

    // синий фреим - добавляю прогноз

    {
        QLabel *lbl = new QLabel(this);
        lbl->setText("ПРОГНОЗ : ");
        new_dac_layout->addWidget(lbl);
    }
    rez_textblock = new QLineEdit(new_dac_frame);
    new_dac_layout->addWidget( rez_textblock );

    // Добавляем пустой эл-т чтобы сдвинуть кнопки
    bottom_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding,
                                    QSizePolicy::Minimum));

    v_main_graph_layout->addWidget(bottom_frame); // добавляем фрейм 2 на основной лэйаут

    wGraphic->clearGraphs();//Если нужно, то очищаем все графики

    //Добавляем графики в widget
    //for (int n=0; n<5; n++)
    {
        wGraphic->addGraph();
    }

    wGraphic->graph(0)->setPen(QPen(Qt::blue));

    // инициализация таймера перерисовки графика
    tmr = new QTimer();
    tmr->setInterval(655);

    connect(tmr, SIGNAL(timeout()), this, SLOT(update_timer()));
}

/*------------------------------------------------------------------*/

void test_membrane::gr_check_box_change()
{
    for (int i=0; i<6; i++)
    {
        board_cfg_pointer->show_graph[i] = Gr_check_box[i]->isChecked();
    }
}

/*------------------------------------------------------------------*/

void test_membrane::start_graph(void)
{
    int status = board_cfg_pointer->start_membrane_graph;



    if (status == 0)
    {
        start();
    }
    else
    {
        stop();
    }
}

/*------------------------------------------------------------------*/

void test_membrane::start()
{
    gr_x.fill(0,gr_buff_len+1);

    gr_y.fill(0,gr_buff_len+1);

    buff_pozition = 0;

    if (!graph_file.isOpen())
    {
        graph_file.setFileName(graph_file_name);
        graph_file.open(QIODevice::WriteOnly);
    }
    tmr->start(655);

    // запуск потока отслеживания мембраны



    board_cfg_pointer->start_membrane_graph = 1;
    start_button->setText("Stop");
}

/*------------------------------------------------------------------*/

void test_membrane::stop()
{
    tmr->stop();

    board_cfg_pointer->start_membrane_graph = 0;

    if (graph_file.isOpen())
    {
        graph_file.close();
    }
    start_button->setText("Start");
}

/*------------------------------------------------------------------*/

void test_membrane::update_timer()
{
    //Проверили что плата передает данные
    if (board_cfg_pointer->State == SGDK::DATA::Connected ||
        board_cfg_pointer->State == SGDK::DATA::Train_detection)
    {
        wGraphic->graph(0)->setData(gr_x,gr_y);

        //Для показа границ по оси Oy надо
        //вычислить мин. и макс. значение в векторах
        maxY = 0;
        minY = 100000;
        a = 100000;
        b= 0;
        for (long int i=1; i<buff_pozition-1; i++)
        {
            // нахожу мин и макс
            if (board_cfg_pointer->show_graph[0])
            {
                if (gr_y[i] < minY) minY = gr_y[i];
                if (gr_y[i] > maxY) maxY = gr_y[i];
            }
        }

        b=gr_x[buff_pozition-1]+1;
        a=gr_x[1]-1;
        if (b<a)
        {
            a=0;
        }

        //Установим область, которая будет показываться на графике
        wGraphic->xAxis->setRange(a, b);//Для оси Ox
        wGraphic->yAxis->setRange(minY-300, maxY+300);//Для оси Oy

        wGraphic->axisRect()->setupFullAxesBox(true);
        wGraphic->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        //И перерисуем график на нашем widget
        wGraphic->replot(QCustomPlot::rpQueuedReplot);
    }
}

/*------------------------------------------------------------------*/

void test_membrane::offset(int offset_len)
{
    minY = gr_y[0];
    maxY = gr_y[0];

    for (long int i=offset_len; i<gr_buff_len+1; i++)
    {
        gr_x[i-offset_len] = gr_x[i];
        gr_y[i-offset_len] = gr_y[i];
    }
}

test_membrane::~test_membrane()
{
}

/*------------------------------------------------------------------*/


} // namespace SGDK
