#include "mainwindow.h"

#include <qcustomplot.h>
#include <QMenuBar>
#include <QPushButton>
#include <QToolButton>
#include <QAction>
#include <QToolBar>
#include <QFrame>
#include <QDockWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QTextBlock>
#include <QString>

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCloseEvent>

#include <QDataStream>
#include <QTcpSocket>
#include <QHostAddress>

#include <QTime>
#include <QTimer>
#include <string>

#include <cstdio>

#include "isa_real_time.h"
#include "save_table_csv.h"
#include "itemeditdialog.h"
#include "graph.h"
#include "test_membrane.h"
#include "edit.h"
#include "brd_table.h"
#include "application.h"
#include "data.h"
#include "start_isa.h"
#include "cfgeditdialog.h"

#include "z_thread.h"
#include "wait.h"

namespace SGDK
{

void MainWindow::set_temp_grad()
{
    APP->pModel->Cfg.temperature_mode = SGDK::DATA::grad;
}

void MainWindow::set_temp_kvant()
{
    APP->pModel->Cfg.temperature_mode = SGDK::DATA::kvant;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    Chanel_group_list << "1-4" << "5-8" << "9-12" << "13-16";

    agModes = new QActionGroup(this);
    agModes->setExclusive(true);
    {
        QAction * A = actMembraneMode = new QAction(this);
        A->setText(tr("Тест мембран"));
        connect(A,SIGNAL(triggered()),this, SLOT(TestMembrane_slot()));
    }
    {
        QAction *A = act_Cfg1 = new QAction(this);
        //QIcon Ic("Dat.ico");      // Добавляю иконку из файла
        //QIcon Ic(":/icons/table");  // Добавляю иконку из ресурса
        //A->setIcon(Ic);                         // Подключаю иконку
        A->setText(tr("1.cfg"));
        A->setCheckable(true);
        A->setChecked(true);
        agModes->addAction(A);
        connect(A,SIGNAL(triggered()),this,SLOT(cfg1_Mode()));
    }
    {
        QAction *A = act_Cfg2 = new QAction(this);
        A->setText(tr("2.cfg"));
        A->setCheckable(true);
        agModes->addAction(A);
        connect(A,SIGNAL(triggered()),this,SLOT(cfg2_Mode()));
    }
    {
        QAction *A = act_edit_cfg = new QAction(this);
        A->setText(tr("App config"));
        A->setCheckable(FALSE);
        agModes->addAction(A);
        connect(A,SIGNAL(triggered()),this,SLOT(edit_app_cfg()));
    }

    {
        QAction *A = act_temp_grad = new QAction(this);
        A->setText(tr("Температура в градусах"));
        A->setCheckable(true);
        A->setChecked(true);
        agModes->addAction(A);
        connect(A,SIGNAL(triggered()),this,SLOT(set_temp_grad()));
    }
    {
        QAction *A = act_temp_kvant = new QAction(this);
        A->setText(tr("Температура в квантах"));
        A->setCheckable(true);
        A->setChecked(false);
        agModes->addAction(A);
        connect(A,SIGNAL(triggered()),this,SLOT(set_temp_kvant()));
    }

    QMenu *mItem = menuBar()->addMenu(tr("Мембраны")); //возвращает указ-ль на меню главного окна (QMenuBar)
    mItem->addAction(actMembraneMode); //добавление в меню

    QMenu *mModes = menuBar()->addMenu(tr("Config"));
    mModes->addAction(act_Cfg1);
    mModes->addAction(act_Cfg2);
    mModes->addAction(act_edit_cfg);

    QMenu *mTemp = menuBar()->addMenu("Температура");
    mTemp->addAction(act_temp_grad);
    mTemp->addAction(act_temp_kvant);

    tbModes = new QToolBar(this);
    //tbModes->setWindowTitle(tr("Modes"));
    tbModes->addAction(act_Cfg1);
    tbModes->addAction(act_Cfg2);
    addToolBar(Qt::TopToolBarArea,tbModes);

    T_widget  = new edit(this);
    setCentralWidget(T_widget);

    QMenu *mWindows = menuBar()->addMenu(tr("View"));

    // 1-й припаркованный виджет - таблица плат
    {
        TableDockWidget = new QDockWidget(this);
        TableDockWidget->setWindowTitle(tr("Yellow"));

        // Добавляю фрейм
        Frame1 = new QFrame(this);
        //F1->setStyleSheet("background: yellow");
        TableDockWidget->setWidget(Frame1);

        // Формирование таблицы списка плат
        Tab = new brd_table(this);
        Tab->resizeColumnsToContents();

        // Добавляю список плат на фрейм
        TableDockWidget->setWidget(Tab);

        addDockWidget(Qt::LeftDockWidgetArea,TableDockWidget);
        mWindows->addAction(TableDockWidget->toggleViewAction());

        connect(Tab,SIGNAL(clicked(QModelIndex)),
                    this,SLOT(fill_property_block()));
    }

    // второй док виджет
    {
        Dw1 = new QDockWidget(this);
        Dw1->setWindowTitle(tr("Log"));

        // Инициализация окна лога
        APP->logTextEdit = new QTextEdit(this);
        APP->logTextEdit->append("Log started:");

        Dw1->setWidget(APP->logTextEdit);
        //D1->setWidget(SGDK::logTextEdit);

        addDockWidget(Qt::BottomDockWidgetArea,Dw1);
        mWindows->addAction(Dw1->toggleViewAction());
        Dw1->hide();
    }

    { // 3-й док виджет  -  кнопки
        Dw2 = new QDockWidget(this);
        Dw2->setWindowTitle(tr("Oscilloscope"));

        addDockWidget(Qt::LeftDockWidgetArea,Dw2);
        mWindows->addAction(Dw2->toggleViewAction());

        //-----------

        // Фрейм с цапами и кнопками
        grfph_frame = new QFrame(this);
        // стиль фрейма
        //grfph_frame->setStyleSheet("background: green");
        //F1->setFrameStyle(StyledPanel | Sunken);
        grfph_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        Dw2->setWidget(grfph_frame); //Добавляю фрейм на док виджет

        //----------

        // после созд. фрейма прикр. к нему лэйаут (создаем разметку внутри фрейма)
        v_main_graph_layout = new QVBoxLayout(grfph_frame);
        v_main_graph_layout->setContentsMargins(0,0,0,0);

        //----------

        //Вспомогательный фрейм с значениями ЦАП
        dac_frame = new QFrame(this);
        // стиль фрейма
        dac_frame->setStyleSheet("background: white");

        //F2->setFrameStyle(StyledPanel | Sunken);
        dac_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
        dac_frame->setFixedHeight(60);

        QVBoxLayout *dac_layout = new QVBoxLayout(dac_frame);
        dac_layout->setContentsMargins(0,0,0,0);

        //помещаю фрейм с цапами на лэйаут с кнопками
        v_main_graph_layout->addWidget(dac_frame);

        //----------

        // создаю 2 фрейма для старых и новых значений ЦАП
        dac_title_frame = new QFrame(this);
        QFrame *old_dac_frame = new QFrame(this);
        QFrame *new_dac_frame = new QFrame(this);
        old_dac_frame->setStyleSheet("background: yellow");
        new_dac_frame->setStyleSheet("background: white");

        //и помещаю их на лэйаут
        dac_layout->addWidget(dac_title_frame);
        dac_layout->addWidget(old_dac_frame);
        dac_layout->addWidget(new_dac_frame);
        //добавляю к ним разметку
        QHBoxLayout *dac_title_layout = new QHBoxLayout(dac_title_frame);
        QHBoxLayout *old_dac_layout = new QHBoxLayout(old_dac_frame);
        QHBoxLayout *new_dac_layout = new QHBoxLayout(new_dac_frame);
        dac_title_layout->setContentsMargins(0,0,0,0);
        old_dac_layout->setContentsMargins(0,0,0,0);
        new_dac_layout->setContentsMargins(0,0,0,0);

        //шапка (ЦАП и №платы)
        {
            for(int i=0;i<4;i++)
            {
                QLabel *lbl = new QLabel(this);

                lbl->setText("ЦАП " + QString::number(i+1));
                lbl->setStyleSheet(APP->pModel->chanel_color[i].color_style);
                lbl->setMaximumWidth(70);
                lbl->setMinimumWidth(70);
                dac_title_layout->addWidget(lbl);
            }
            QLabel *lbl = new QLabel(this);
            lbl->setText("Brd №");
            lbl->setMargin(0);
            lbl->setMaximumWidth(70);
            lbl->setMinimumWidth(70);
            dac_title_layout->addWidget(lbl);

            lbl = new QLabel(this);
            lbl->setText("Частота");
            lbl->setMargin(0);
            lbl->setMaximumWidth(70);
            lbl->setMinimumWidth(70);
            dac_title_layout->addWidget(lbl);

            lbl_chanal = new QLabel(this);
            lbl_chanal->setText("Каналы");
            lbl_chanal->setMargin(0);
            lbl_chanal->setMaximumWidth(70);
            lbl_chanal->setMinimumWidth(70);
            dac_title_layout->addWidget(lbl_chanal);
            lbl_chanal->setVisible(false);

            lbl = new QLabel(this);
            lbl->setText("T-limit");
            lbl->setMargin(0);
            lbl->setMaximumWidth(70);
            lbl->setMinimumWidth(70);
            dac_title_layout->addWidget(lbl);

        }
        dac_title_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum) );

        //заполняю поля цап значениями
        old_dac0_textblock = new QLineEdit(old_dac_frame);
        old_dac1_textblock = new QLineEdit(old_dac_frame);
        old_dac2_textblock = new QLineEdit(old_dac_frame);
        old_dac3_textblock = new QLineEdit(old_dac_frame);
        brd_nomb_textblock = new QLineEdit(old_dac_frame);
        freq_textblock     = new QLineEdit(old_dac_frame);
        temper_limit_textblock = new QLineEdit(old_dac_frame);
        QComboBox *chanel_group  = new QComboBox(old_dac_frame);

        old_dac0_textblock->setContentsMargins(0,0,0,0);
        old_dac0_textblock->setMaximumWidth(70);
        old_dac1_textblock->setContentsMargins(0,0,0,0);
        old_dac1_textblock->setMaximumWidth(70);
        old_dac2_textblock->setContentsMargins(0,0,0,0);
        old_dac2_textblock->setMaximumWidth(70);
        old_dac3_textblock->setContentsMargins(0,0,0,0);
        old_dac3_textblock->setMaximumWidth(70);
        brd_nomb_textblock->setContentsMargins(0,0,0,0);
        brd_nomb_textblock->setMaximumWidth(70);
        freq_textblock->setContentsMargins(0,0,0,0);
        freq_textblock->setMaximumWidth(70);
        temper_limit_textblock->setContentsMargins(0,0,0,0);
        temper_limit_textblock->setMaximumWidth(70);

        chanel_group->setContentsMargins(0,0,0,0);

        old_dac0_textblock->setText("-1");
        old_dac1_textblock->setText("-1");
        old_dac2_textblock->setText("-1");
        old_dac3_textblock->setText("-1");
        brd_nomb_textblock->setText("-1");
        freq_textblock  ->setText("-1");
        temper_limit_textblock->setText("-1");

        chanel_group->addItems(Chanel_group_list);

        chanel_group->setCurrentIndex(0);
        chanel_group->setMaximumWidth(70);

        chanel_group->setVisible(false);
        chanel_group->setDisabled(true);

        connect(chanel_group,SIGNAL(currentIndexChanged(int)),this, SLOT(chanel_group_changed()));

        old_dac_layout->addWidget(old_dac0_textblock);
        old_dac_layout->addWidget(old_dac1_textblock);
        old_dac_layout->addWidget(old_dac2_textblock);
        old_dac_layout->addWidget(old_dac3_textblock);
        old_dac_layout->addWidget(brd_nomb_textblock);
        old_dac_layout->addWidget(freq_textblock);
        old_dac_layout->addWidget(temper_limit_textblock);
        old_dac_layout->addWidget(chanel_group);
        old_dac_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum) );

        //new_dac_layout
        new_dac0_textblock = new QLineEdit(new_dac_frame);
        new_dac1_textblock = new QLineEdit(new_dac_frame);
        new_dac2_textblock = new QLineEdit(new_dac_frame);
        new_dac3_textblock = new QLineEdit(new_dac_frame);
        new_nomb_textblock = new QLineEdit(new_dac_frame);
        new_freq_textblock = new QLineEdit(new_dac_frame);
        new_temper_limit_textblock = new QLineEdit(new_dac_frame);

        new_dac0_textblock->setContentsMargins(0,0,0,0);
        new_dac0_textblock->setMaximumWidth(70);
        new_dac1_textblock->setContentsMargins(0,0,0,0);
        new_dac1_textblock->setMaximumWidth(70);
        new_dac2_textblock->setContentsMargins(0,0,0,0);
        new_dac2_textblock->setMaximumWidth(70);
        new_dac3_textblock->setContentsMargins(0,0,0,0);
        new_dac3_textblock->setMaximumWidth(70);
        new_nomb_textblock->setContentsMargins(0,0,0,0);
        new_nomb_textblock->setMaximumWidth(70);
        new_freq_textblock->setContentsMargins(0,0,0,0);
        new_freq_textblock->setMaximumWidth(70);
        new_temper_limit_textblock->setContentsMargins(0,0,0,0);
        new_temper_limit_textblock->setMaximumWidth(70);

        new_dac0_textblock->setText("");
        new_dac1_textblock->setText("");
        new_dac2_textblock->setText("");
        new_dac3_textblock->setText("");
        new_nomb_textblock->setText("");
        new_freq_textblock->setText("");
        new_temper_limit_textblock->setText("");

        new_dac_layout->addWidget(new_dac0_textblock);
        new_dac_layout->addWidget(new_dac1_textblock);
        new_dac_layout->addWidget(new_dac2_textblock);
        new_dac_layout->addWidget(new_dac3_textblock);
        new_dac_layout->addWidget(new_nomb_textblock);
        new_dac_layout->addWidget(new_freq_textblock);
        new_dac_layout->addWidget(new_temper_limit_textblock);

        // Добавляем отступ
        new_dac_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum) );

        QFrame *bottom_frame = new QFrame(this);
        QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_frame);
        bottom_layout->setContentsMargins(0,0,0,0);
        bottom_layout->setSpacing(0);
        {       // ------- кнопка START ------ //
            // вспомогательный Фрейм с кнопками

            //bottom_frame->setStyleSheet("background: red");
            //F2->setFrameStyle(StyledPanel | Sunken);
            bottom_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
            bottom_frame->setFixedHeight(30);

            start_button = new QPushButton(this);
            start_button->setText("Граф.");
            start_button->setFixedWidth(60);

            bottom_layout->addWidget(start_button);
            connect(start_button, SIGNAL(clicked()),this, SLOT(start_graph()));
        }
        {       // ------- кнопка SET ------ //
            QPushButton *set_button =  new QPushButton(this);
            set_button->setText("Установка");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(set_button);
            connect(set_button, SIGNAL(clicked()),this, SLOT(set_brd_param()));
        }

        // Добавляем отступ
        bottom_layout->addItem( new QSpacerItem(10,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );

        {   // ------- кнопка SEND_MEANDR ------ //

            param_textblock = new QLineEdit(bottom_frame);
            param_textblock->setMinimumSize(100,10);
            param_textblock->setMaximumSize(100,20);
            bottom_layout->addWidget(param_textblock);

            //добавляю кнопку SEND_MEANDR
            QPushButton *send_meandr_button =  new QPushButton(this);
            QIcon Icm("meandr.ico");      // Добавляю иконку из файла
            send_meandr_button->setIcon(Icm);
            send_meandr_button->setToolTip("Генерация меандра");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(send_meandr_button);
            connect(send_meandr_button, SIGNAL(clicked()),this, SLOT(send_meandr_cmd()));
        }

        {   // кнопка АВТОКАЛИБРОВКИ
            QPushButton *send_kalibr_button =  new QPushButton(this);
            QIcon Ick("kalibr.ico");      // Добавляю иконку из файла
            send_kalibr_button->setIcon(Ick);
            send_kalibr_button->setToolTip("Автокалибровка");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(send_kalibr_button);
            connect(send_kalibr_button, SIGNAL(clicked()),this, SLOT(send_kalibr_cmd()));
        }

        {       // кнопка ПЕРЕЗАГРУЗКИ
            QPushButton *send_restart_button =  new QPushButton(this);
            QIcon Icp("power.ico");      // Добавляю иконку из файла
            send_restart_button->setIcon(Icp);
            send_restart_button->setToolTip("Сброс питания");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(send_restart_button);
            connect(send_restart_button, SIGNAL(clicked()),this, SLOT(send_restart_cmd()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(30,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {       // кнопка 2кгц
            QPushButton *send_2khz_button =  new QPushButton(this);
            //QIcon Icp("power.ico");      // Добавляю иконку из файла
            send_2khz_button->setText("2кГц");
            //send_2khz_button->setIcon(Icp);
            send_2khz_button->setToolTip("Переключение всех доступных плат на 2кГц");
            send_2khz_button->setFixedWidth(50);
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(send_2khz_button);
            connect(send_2khz_button, SIGNAL(clicked()),this, SLOT(send_2kHz_cmd()));
            // Добавляем отступ
            //bottom_layout->addItem( new QSpacerItem(30,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {       // кнопка 4кгц
            QPushButton *send_4khz_button =  new QPushButton(this);
            QIcon Icp("power.ico");      // Добавляю иконку из файла
            //send_4khz_button->setIcon(Icp);
            send_4khz_button->setText("4кГц");
            send_4khz_button->setToolTip("Переключение всех доступных плат на 4кГц");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(send_4khz_button);
            send_4khz_button->setFixedWidth(50);
            connect(send_4khz_button, SIGNAL(clicked()),this, SLOT(send_4kHz_cmd()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(10,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }


        {       // кнопка СДЕЛАТЬ ВСЕ
            QPushButton *calibrate_all_button =  new QPushButton(this);
            calibrate_all_button->setText("Сделать все");
            calibrate_all_button->setToolTip("Калибровка системы");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(calibrate_all_button);
            connect(calibrate_all_button, SIGNAL(clicked()),this, SLOT(calibrate_all_cmd()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(10,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {       // кнопка АВТОНУМЕРАЦИЯ
            QPushButton *autonum_button =  new QPushButton(this);
            autonum_button->setText("Автонум");
            autonum_button->setToolTip("Автонумерация плат");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(autonum_button);
            connect(autonum_button, SIGNAL(clicked()),this, SLOT(autonum_cmd()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(30,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {   // ------- отправка произвольной команды ------ //
            command_textblock = new QLineEdit(bottom_frame);
            command_textblock->setMinimumSize(100,10);
            command_textblock->setMaximumSize(100,20);
            bottom_layout->addWidget(command_textblock);
            send_cmd_button = new QPushButton(this);
            send_cmd_button->setText("Команда");
            bottom_layout->addWidget(send_cmd_button);
            connect(send_cmd_button, SIGNAL(clicked()),this, SLOT(send_cmd_butt_press()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(30,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {   // ------- отправка однобайтной команды команды ------ //
            send_cmd_button = new QPushButton(this);
            send_cmd_button->setText("Чистка буф");
            send_cmd_button->setToolTip("Очистка буфера команд - нажимать 1-4 раза");
            bottom_layout->addWidget(send_cmd_button);
            connect(send_cmd_button, SIGNAL(clicked()),this, SLOT(send_short_cmd_butt_press()));
            // Добавляем отступ
            bottom_layout->addItem( new QSpacerItem(10,0,QSizePolicy::Minimum, QSizePolicy::Minimum) );
        }

        {   // ------- кнопка SAVE ------ //
            QPushButton *save_table_button =  new QPushButton(this);
            QIcon Icm("save.ico");      // Добавляю иконку из файла
            save_table_button->setIcon(Icm);
            save_table_button->setToolTip("Сохранить таблицу");
            // Добавляем кнопку на лэйаут
            bottom_layout->addWidget(save_table_button);
            connect(save_table_button, SIGNAL(clicked()),this, SLOT(save_table_cmd()));
            // Добавляем пустой эл-т чтобы сдвинуть кнопки
            bottom_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum) );
        }

        // ------------------------

        v_main_graph_layout->addWidget(bottom_frame); // добавляем фрейм 2 на основной лэйаут

        // Добавляем пустой эл-т чтобы прижать все к верху
        v_main_graph_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding) );
    }

    { /* центральный виджет */ }


    //state.RestoreState();

    //this->move(pos);
    this->resize(APP->pModel->Cfg.main_win_size_x,APP->pModel->Cfg.main_win_size_y);




    // Инициализация таймера
    connect(&tmr, SIGNAL(timeout()), this, SLOT(updateTime()));
    tmr.start(APP->pModel->Cfg.update_timer_interval_normal);
    //ch_tmr_mode(TIMER_MODES::TMR_NORMAL);
}

/*--------------------------------------------------------------*/

MainWindow::~MainWindow()
{
    //state.SaveState();


    delete T_widget; // Удаляю виджет с выводом лога
    delete act_Cfg1;
    delete actMembraneMode;
    //delete act_Cfg2;
}

/*------------------------------------------------------------*/




void MainWindow::save_table_cmd()
{
    SGDK::save_table_csv(APP->pModel);
}

/*--------------------------------------------------------------*/

void MainWindow::edit_app_cfg()
{
    //создаем диалог
    cfgEditDialog Dia(this);     //т.к. модальный, то можно его сделать автоматически

    //Присоединяем датасет к диалогу (данные надо присоединить к фрейму, т.к. поля все нах  фрейм инкапсулирован в диалог,
    // то присоединяем к диалогу а затем к фрейму
    Dia.attach();

    if (Dia.exec() == QDialog::Accepted)
    {
        //APP->model()->Edit(X);  // запись в БД
        APP->save_config();
    }


    // обновляю таймер перерисовки таблицы, т.к это значение могло измениться

    ch_tmr_mode(TIMER_MODES::TMR_NORMAL);

}



/*--------------------------------------------------------------*/

void MainWindow::chanel_group_changed()  // Слот обработки изменения группы каналов АЦП/ЦАП
{

    // проверка выбрана ли плата и каков ее статус
    QItemSelectionModel *select = Tab->selectionModel();
    QModelIndexList indexes;
    //QModelIndex index;
    if (select->hasSelection()) //check if has selection
    {
        indexes = select->selectedIndexes();
        //int row_id = indexes.first().row();

        // определяю кол-во каналов на выбранной плате


    }

}

/*--------------------------------------------------------------*/

void MainWindow::autonum_cmd()
{
    send_cmd_buff[0] = 0xA0;
    send_cmd_buff[1] = 0x08;
    //send_cmd_buff[2] = num >> 8;
    send_cmd_buff[3] = 0;

    //Цикл по всем платам

    for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
    {
        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Train_detection)
        {
            // Определяю № платы по IP
            int n = APP->pModel->Cfg.BrdConfVector[id]->ip[3] - 20;
            if (n > 0)
            {
                send_cmd_buff[2] = n;
                int result = send(APP->pModel->Cfg.BrdConfVector[id]->s ,send_cmd_buff , 4, 0);
                if (result == SOCKET_ERROR || result < 4)
                {
                    // произошла ошибка при отправке данных
                    // closesocket(listen_socket);
                    // closesocket(client_socket);
                    // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                    // APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                    // err = true;
                    // ready_to_delete = true;
                    // return;
                }
            }
        }
    }

}

/*--------------------------------------------------------------*/

void MainWindow::send_meandr_cmd(void)
{
    int num = 0;
    send_cmd_buff[0] = 0xED;
    bool success = false;
    QString command_str = param_textblock->text();
    num = command_str.toInt(&success);
    if(!success)  num = 1000;

    send_cmd_buff[2] = num >> 8;
    send_cmd_buff[3] = num;

    send_cmd();
}

/*--------------------------------------------------------------*/

void MainWindow::send_restart_cmd(void)
{
    send_cmd_buff[0] = 0xEA;
    send_cmd_buff[1] = 0;
    send_cmd_buff[2] = 0;
    send_cmd_buff[3] = 0;

    send_cmd();
}

/*--------------------------------------------------------------*/

void MainWindow::calibrate_all_cmd(void)
{
    send_cmd_buff[0] = 0xEE;

    bool success = false;
    QString command_str = param_textblock->text();
    int num = command_str.toInt(&success);
    if(!success)   num = 40000;
    if(num < 35000)   num = 40000;

    send_cmd_buff[2] = num >> 8;
    send_cmd_buff[3] = num;

    //Цикл по всем платам

    for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
    {
        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Train_detection)
        {

            int result = send(APP->pModel->Cfg.BrdConfVector[id]->s ,send_cmd_buff , 4, 0);
            if (result == SOCKET_ERROR || result < 4)
            {
                // произошла ошибка при отправке данных
                // closesocket(listen_socket);
                // closesocket(client_socket);
                // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                // APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                // err = true;
                // ready_to_delete = true;
                // return;
            }
        }
    }

}

/*--------------------------------------------------------------*/

void MainWindow::send_kalibr_cmd(void)
{
    int num = 0;
    send_cmd_buff[0] = 0xEE;
    bool success = false;
    QString command_str = param_textblock->text();
    num = command_str.toInt(&success);
    if(!success)   num = 40000;

    send_cmd_buff[2] = num >> 8;
    send_cmd_buff[3] = num;

    send_cmd();
}

/*--------------------------------------------------------------*/

void MainWindow::fill_property_block()  // слот срабатывает по нажатию на строке таблицы
{
    // проверка выбрана ли плата и каков ее статус
    QItemSelectionModel *select = Tab->selectionModel();
    QModelIndexList indexes;

    if (select->hasSelection()) //check if has selection
    {
        indexes = select->selectedIndexes();

        int row_id = indexes.first().row();

        fill_dac_cell(old_dac0_textblock, row_id, 4);
        fill_dac_cell(old_dac1_textblock, row_id, 5);
        fill_dac_cell(old_dac2_textblock, row_id, 6);
        fill_dac_cell(old_dac3_textblock, row_id, 7);
        fill_dac_cell(brd_nomb_textblock, row_id, 0);
        fill_dac_cell(freq_textblock, row_id, 16);
        fill_dac_cell(temper_limit_textblock, row_id, 30);
    }
}

/*--------------------------------------------------------------*/

void MainWindow::fill_dac_cell(QLineEdit* field, int row_id, int col_id)
{
    QModelIndex index;
    QVariant Data_str;

    index    = Tab->model()->index(row_id,col_id);
    Data_str = Tab->model()->data(index);

    //bool ok = true;
    //int dat = Data_str.toInt(&ok);
    //if(ok) field->setText(QString::number(dat));
    field->setText(Data_str.toString());
}

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

void MainWindow::send_2kHz_cmd()
{
    send_cmd_buff[0] = 0xA0;
    send_cmd_buff[1] = 0x08;
    send_cmd_buff[2] = 0;
    send_cmd_buff[3] = 2;

    //Цикл по всем платам

    for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
    {
        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Train_detection)
        {

            int result = send(APP->pModel->Cfg.BrdConfVector[id]->s ,send_cmd_buff , 4, 0);
            if (result == SOCKET_ERROR || result < 4)
            {
                // произошла ошибка при отправке данных
                // closesocket(listen_socket);
                // closesocket(client_socket);
                // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                // APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                // err = true;
                // ready_to_delete = true;
                // return;
            }
        }
    }
}

/*--------------------------------------------------------------*/

void MainWindow::send_4kHz_cmd()
{
    send_cmd_buff[0] = 0xA0;
    send_cmd_buff[1] = 0x08;

    send_cmd_buff[2] = 0;
    send_cmd_buff[3] = 4;

    //Цикл по всем платам

    for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
    {
        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[id]->State == SGDK::DATA::Train_detection)
        {

            int result = send(APP->pModel->Cfg.BrdConfVector[id]->s ,send_cmd_buff , 4, 0);
            if (result == SOCKET_ERROR || result < 4)
            {
                // произошла ошибка при отправке данных
                // closesocket(listen_socket);
                // closesocket(client_socket);
                // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                // APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                // err = true;
                // ready_to_delete = true;
                // return;
            }
        }
    }
}

/*--------------------------------------------------------------*/

void MainWindow::send_short_cmd_butt_press(void)
{
    // остановка таймера
    tmr.stop();          // похоже ничего не дает
    char cmd_buf[4] = {0};

    // проверка выбрана ли плата и каков ее статус
    QItemSelectionModel *select = Tab->selectionModel();
    QModelIndexList indexes;
    //QModelIndex index;
    if (select->hasSelection()) //check if has selection
    {
        indexes = select->selectedIndexes();
        int row_id = indexes.first().row();

        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[row_id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[row_id]->State == SGDK::DATA::Train_detection)
        {

            int result = send(APP->pModel->Cfg.BrdConfVector[row_id]->s ,cmd_buf , 1, 0);
            if (result == SOCKET_ERROR || result < 4)
            {
                // произошла ошибка при отправке данных
                //closesocket(listen_socket);
                // closesocket(client_socket);
                // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                //APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                // err = true;
                // ready_to_delete = true;
                // return;
            }
        }
    }

    Sleep(800);

    ch_tmr_mode(TIMER_MODES::TMR_NORMAL);
}

/*--------------------------------------------------------------*/


// ------- отправка произвольной команды на МК -------
void MainWindow::send_cmd_butt_press(void)
{
    // проверка введенных данных, формирование команды
    bool success = false;
    int num;
    QString S;

    for (int i = 0; i<4; i++)
    {
        S=command_textblock->text().section(' ',i,i); // 1-й параметр - команда
        num = S.toInt(&success,16);
        if(!success)   // Преобразование прошло успешно
        {
            num = 0;
        }
        send_cmd_buff[i] = num;
    }

    // отправка команды
    send_cmd();
}

/*--------------------------------------------------------------*/

void MainWindow::send_cmd()
{    
    // остановка таймера
    tmr.stop();          // похоже ничего не дает

    // проверка выбрана ли плата и каков ее статус
    QItemSelectionModel *select = Tab->selectionModel();
    QModelIndexList indexes;
    //QModelIndex index;
    if (select->hasSelection()) //check if has selection
    {
        indexes = select->selectedIndexes();
        int row_id = indexes.first().row();

        //Проверили что плата передает данные
        if (APP->pModel->Cfg.BrdConfVector[row_id]->State == SGDK::DATA::Connected ||
            APP->pModel->Cfg.BrdConfVector[row_id]->State == SGDK::DATA::Train_detection)
        {

            int result = send(APP->pModel->Cfg.BrdConfVector[row_id]->s ,send_cmd_buff , 4, 0);
            if (result == SOCKET_ERROR || result < 4)
            {
                // произошла ошибка при отправке данных
                //closesocket(listen_socket);
                // closesocket(client_socket);
                // WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.
                //APP->pModel->Cfg.BrdConfVector[row_id].State = SGDK::DATA::Disconnected;
                // err = true;
                // ready_to_delete = true;
                // return;
            }
        }
    }

    Sleep(1000);

    ch_tmr_mode(TIMER_MODES::TMR_NORMAL);
}

/*--------------------------------------------------------------*/

void MainWindow::set_brd_param(void)
{
    send_cmd_to_addr(0, new_dac0_textblock->text());
    //Sleep(10);
    send_cmd_to_addr(2, new_dac1_textblock->text());
    //Sleep(10);
    send_cmd_to_addr(4, new_dac2_textblock->text());
    //Sleep(10);
    send_cmd_to_addr(6, new_dac3_textblock->text());
    //Sleep(10);
    send_cmd_to_addr(8, new_nomb_textblock->text(),1);
    //Sleep(10);
    send_cmd_to_addr(8, new_freq_textblock->text(),2);
    //Sleep(10);
    send_cmd_to_addr(54, new_temper_limit_textblock->text());
    //Sleep(10);
}

/*--------------------------------------------------------------*/

void MainWindow::send_cmd_to_addr(int addr, QString command_str, int byte_to_send, int cmd)
// byte_to_send - 0- отправка двух байт, 1 -только первый, 2-только второй
{
    //формирую компнду для отправки на плату
    send_cmd_buff[0] = cmd;  // Команда записи в Flash

    int num = 0;
    bool success = false;
    num = command_str.toInt(&success);
    if(success)   // Преобразование прошло успешно
    {
        //формирую команду для отправки на МК
        send_cmd_buff[1] = addr;     // Адрес ячейки памяти платы

        if (byte_to_send == 0)
        {
            send_cmd_buff[2] = num >> 8;
            send_cmd_buff[3] = num;
        }
        else if (byte_to_send == 1)
        {
            send_cmd_buff[2] = num;
            send_cmd_buff[3] = 0;
        }
        else if (byte_to_send == 2)
        {
            send_cmd_buff[2] = 0;
            send_cmd_buff[3] = num;
        }
        send_cmd();
    }
}

/*--------------------------------------------------------------*/

void MainWindow::closeEvent(QCloseEvent* event)
{
    stop_all();
    Sleep(800); // Жду пока все соединения закроются

    event->accept();  //для разрешения закрытие окна
}

/*--------------------------------------------------------------*/

void MainWindow::stop_all()
{
    // Закрытие всех дочерних окон
    for (unsigned int i=0; i<APP->pModel->Cfg.BrdConfVector.size(); i++)
    {
        if( APP->pModel->Cfg.BrdConfVector[i]->gr_win.graph_win_is_open  )
        {
            APP->pModel->Cfg.BrdConfVector[i]->gr_win.close();
            //wait_until_true( &(APP->pModel->Cfg.BrdConfVector[i]->gr_win.graph_win_is_open) );
            //delete APP->pModel->Cfg.BrdConfVector[i]->gr_w;
            //APP->pModel->Cfg.BrdConfVector[i]->gr_win = nullptr;
        }
    }

    // остановка всех потоков
    stop_all_threads();
    //Sleep(50);
}

/*------------------------------------------------------------*/

void MainWindow::stop_all_threads()
{
    for (unsigned int i=0; i<APP->pModel->Cfg.BrdConfVector.size(); i++)
    {
        APP->pModel->Cfg.BrdConfVector[i]->State = SGDK::DATA::Off;
        APP->pModel->Cfg.BrdConfVector[i]->isa_connection_status = false;

    }

}

/*------------------------------------------------------------*/

void MainWindow::start_graph(void)
{
    // проверка выбрана ли плата и каков ее статус
    QItemSelectionModel *select = Tab->selectionModel();
    QModelIndexList indexes;
    //QModelIndex index;
    if (select->hasSelection()) //check if has selection
    {
        indexes = select->selectedIndexes();

        graph_brd_row_nomb = indexes.first().row();

        if (!APP->pModel->Cfg.BrdConfVector[graph_brd_row_nomb]->gr_win.graph_is_started)
        {
            brd_nomb_textblock->setText(QString::number(APP->pModel->Cfg.BrdConfVector[graph_brd_row_nomb]->BrdNomb));

            DATA::BoardCfg* board_cfg_pointer = APP->pModel->Cfg.BrdConfVector[graph_brd_row_nomb];

            // открываем окно диалога
            board_cfg_pointer->gr_win.graph_window_init( board_cfg_pointer );

            //board_cfg_pointer->gr_win.setParent(QApplication::activeWindow());

            board_cfg_pointer->gr_win.setModal(false);
            board_cfg_pointer->gr_win.setWindowFlags(board_cfg_pointer->gr_win.windowFlags() | Qt::WindowStaysOnTopHint);

            board_cfg_pointer->gr_win.show();
            board_cfg_pointer->gr_win.resize(600,500);

            board_cfg_pointer->gr_win.graph_win_is_open = true;

        }
    }
}

/*------------------------------------------------------------*/

//void MainWindow::delete_closed_gr_wins(DATA::BoardCfg *cfg_pointer)
//{

//    if(cfg_pointer->gr_win_ready_to_delete)
//    {
        //delete cfg_pointer->gr_w;
        //cfg_pointer->gr_w = nullptr;
//    }

//}

/*------------------------------------------------------------*/

void MainWindow::delete_closed_membrane_wins(DATA::BoardCfg *cfg_pointer)
{
 /*   if (APP->membrane_wins.contains(cfg_pointer))
    {
        if (APP->membrane_wins[cfg_pointer]->ready_to_delete == true)
        {
            test_membrane *mw = APP->membrane_wins[cfg_pointer];

            delete mw;
            APP->membrane_wins.remove(cfg_pointer);
        }
    }  */
}

/*------------------------------------------------------------*/

void MainWindow::ch_tmr_mode(TIMER_MODES mode)
{
    // перезапуск таймера в высоконагруженном режиме

    // остановка таймера
    tmr.stop();
    if (mode == TIMER_MODES::TMR_NORMAL)
    {
        tmr.start(APP->pModel->Cfg.update_timer_interval_normal);
        timer_mode = TIMER_MODES::TMR_NORMAL;
    }
    else
    {
        tmr.start(APP->pModel->Cfg.update_timer_interval_max);
        timer_mode = TIMER_MODES::TMR_MAX_INTERVAL;
    }


}

void MainWindow::updateTime()   // Обработчик таймера запуска захватов
{
    // Цикл по всем платам из списка
    for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
    {
        DATA::BoardCfg *cfg_pointer = APP->pModel->Cfg.BrdConfVector[id];

        // Запуск сервиса захвата если он еще не запущен но имеет статус ON

        if (cfg_pointer->State == SGDK::DATA::On)
        {
            //create_new_z_thread(cfg_pointer);
            if (cfg_pointer->z_thr.z_run == false)
            {
                cfg_pointer->z_thr.start();
            }
        }
        else if (cfg_pointer->State == SGDK::DATA::Disconnected)
        {
            // Подключение не удалось, запускаю счетчик ожидания повторного запуска
            reconnect_after_pause(cfg_pointer);
        }
        //else if (cfg_pointer->State == SGDK::DATA::Off)
        //{
        //}

        // Уничтожаю объект потока захвата для платы если он помечен на удаление
        //kill_z_thread_by_addr_if_ready(cfg_pointer);

        // проверяю нет ли закрытых но не уничтоженых окон с графиками
        //delete_closed_gr_wins(cfg_pointer);

        // проверяю нет ли закрытых но не уничтоженых окон тестами мембран
        //delete_closed_membrane_wins(cfg_pointer);
    };

    // проверяю нет ли обработанных вагонов. если есть то усредняю и вывожу на экран
    if (APP->wheels_buff.size() > 0)
    {
        average_wheels();
        show_train();  // вывод на экран всех колес с весом
    }

    // проверяю завершение проезда по всем платам
    {
        int proc_count = 0;
        for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
        {
            DATA::BoardCfg *cfg_pointer = APP->pModel->Cfg.BrdConfVector[id];

            if ( cfg_pointer->train_started && (!cfg_pointer->train_stopped) )
            {
                // есть начатые и  незавершенные обработки
                proc_count ++;
            }
        }
        if (proc_count == 0)
        {
            // все начатые проезды завершены
            for (unsigned int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
            {
                // сброс всех флагов
                APP->pModel->Cfg.BrdConfVector[id]->train_started = false;
                APP->pModel->Cfg.BrdConfVector[id]->train_stopped = false;
            }

            // Если записано слишком много проездов то удаляю старые
            QDir dir(APP->pModel->Cfg.archiv_dir);
            QFileInfoList dir_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);

            while(dir_list.size() > APP->pModel->Cfg.max_arhive_nomb)
            {
                //qDebug() << "Removed last element" << files.last().filePath();
                QString DirName = dir_list.takeLast().filePath();
                QDir dir_to_delete(DirName);
                dir_to_delete.removeRecursively();
            }

            APP->pModel->Cfg.archive_folder_created = false;

            // перезапуск таймера в штатном режиме
            ch_tmr_mode(TIMER_MODES::TMR_NORMAL);

        }
        else  // есть что обрабатывать
        {
            // перезапуск таймера в высоконагруженном режиме
            ch_tmr_mode(TIMER_MODES::TMR_MAX_INTERVAL);
        }
    }
    // запускаю процесс, проверяющий нет ли подготовленных для Исаева файлов и передающий эти файлы
    if(!start_isa::active)
    {
        start_isa::active = true;
        //isa.z_boards_map_pointer = &z_boards_map;

        isa.start();
    }
    Tab->update_values(); // обновляю данные в табличке
}

/*--------------------------------------------------------------------*/

//void MainWindow::create_new_z_thread(DATA::BoardCfg *brd_cfg_pointer)
//{   //Создаю объект класса z_thread и помещаю в map где содержатся все активные захваты
    /*if (z_boards_map.find(brd_cfg_pointer) == z_boards_map.end())  //еще нет в словаре такого элемента
    {
        Z_thread *Z = new Z_thread(brd_cfg_pointer);

        // добавляю в словарь
        z_boards_map[brd_cfg_pointer] = Z;

        // запускаю поток захвата объекта Z
        Z->start();
    }
    else // плата с таким номером уже есть
    {
        // пишу в лог
        int st = brd_cfg_pointer->State;
        //bool ready_to_del = z_boards_map[id]->ready_to_delete;

        std::string S = "Board №" + std::to_string(brd_cfg_pointer->BrdNomb) + " already run"+
                        " state=" + std::to_string(st);

        // если статус On_ то удаляю из словаря
        //if ((st==1) && (ready_to_del==false))
        //{
        //    kill_z_thread_by_id_if_ready(id);

        //}

        SGDK::DATA::config::write_log_all(S);
    } */
//}

/*--------------------------------------------------------------------*/

//void MainWindow::reconnect_after_pause(int id)

void MainWindow::reconnect_after_pause(DATA::BoardCfg *brd_cfg_pointer)
{
    //int max_to = APP->pModel->Cfg.BrdConfVector[id]->max_timeout;
    if (brd_cfg_pointer->timeout_counter++ > brd_cfg_pointer->max_timeout)

    {
        brd_cfg_pointer->timeout_counter = 0;
        brd_cfg_pointer->State = SGDK::DATA::On;

        //std::string S = "Board №" + std::to_string(APP->pModel->Cfg.BrdConfVector[id].BrdNomb ) + " retry";
        //SGDK::DATA::config::write_log_all(S);
    }
}

/*--------------------------------------------------------------------*/

//void MainWindow::kill_z_thread_by_addr_if_ready(DATA::BoardCfg *brd_cfg_pointer)
//{
    /*if (z_boards_map.find(brd_cfg_pointer) != z_boards_map.end())
    {
        if(z_boards_map[brd_cfg_pointer]->ready_to_delete)
        {
            brd_cfg_pointer->z_thread_status=60;
            delete z_boards_map[brd_cfg_pointer];    //удаляю объект
            z_boards_map.erase(brd_cfg_pointer);     //удаляю запись в map
        }
    }*/
//}

/*--------------------------------------------------------------------*/

void MainWindow::average_wheels()
{
    //заранее резервирую место в векторе
    //APP->wheels_all.reserve(APP->wheels_buff.size()+1);
    while (APP->wheels_buff.size() > 0)
    {
        // надо заполнить APP->wheels_all из APP->wheels_buff
        wheel_rez_data dat = APP->wheels_buff.back();
        APP->wheels_buff.pop_back(); // удаляем прочитанный элемент
        APP->wheels_all.push_back(dat);

        // потом усредняю и заполняю wheels_average
        if ( APP->wheels_average.find(dat.wheel_nomb) != APP->wheels_average.end() )
        {
            // добавляю
            if (dat.wheel_rez_data_brd_cfg_pointer->side == 0) // правая сторона
            {
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_r++;
                APP->wheels_average[dat.wheel_nomb].weigth_r = APP->wheels_average[dat.wheel_nomb].weigth_r + dat.weigth_r;
            }
            else
            {
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_l++;
                APP->wheels_average[dat.wheel_nomb].weigth_l = APP->wheels_average[dat.wheel_nomb].weigth_l + dat.weigth_l;
            }
        }
        else
        {
            APP->wheels_average[dat.wheel_nomb] = dat;
            if (dat.wheel_rez_data_brd_cfg_pointer->side == 0) // правая сторона
            {
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_r = 1;
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_l = 0;

                APP->wheels_average[dat.wheel_nomb].weigth_r = dat.weigth_r;
                APP->wheels_average[dat.wheel_nomb].weigth_l = 0;
            }
            else
            {
                //APP->wheels_average[dat.wheel_nomb] = dat;
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_l = 1;
                APP->wheels_average[dat.wheel_nomb].measurement_nomb_r = 0;

                APP->wheels_average[dat.wheel_nomb].weigth_l = dat.weigth_l;
                APP->wheels_average[dat.wheel_nomb].weigth_r = 0;
            }
        }
    }
}

/*--------------------------------------------------------------------*/

void MainWindow::show_train()
{
    //создаю итератор для перебора всех элементов
    std::map <int, wheel_rez_data> :: iterator iter = APP->wheels_average.begin();

    // вывод на экран колес
    T_widget->clear();
    for(int i=0; iter!=APP->wheels_average.end(); iter++, i++)
    {
        QString S = "Wheel-"  + QString::number(iter->first)
                  + " Van-"   + QString::number(iter->second.van_nomb)
                  + " Weigth_r "+ QString::number(iter->second.weigth_r/iter->second.measurement_nomb_r)
                  + " Weigth_l "+ QString::number(iter->second.weigth_l/iter->second.measurement_nomb_l);
        T_widget->append(S);
    }
}

/*--------------------------------------------------------------------*/

void MainWindow::onReadyRead() { }

/*--------------------------------------------------------------------*/

void MainWindow::TestMembrane_slot()
{
    // проверка выбрана ли плата и каков ее статус
//    QItemSelectionModel *select = Tab->selectionModel();
//    QModelIndexList indexes;
    //QModelIndex index;
//    if (select->hasSelection()) //check if has selection
//    {
//        indexes = select->selectedIndexes();
//        int membrane_brd_row_nomb = indexes.first().row();;

//        if (APP->pModel->Cfg.BrdConfVector[membrane_brd_row_nomb]->membrane_mode_started ==0)
//        {
//            DATA::BoardCfg* board_cfg_pointer = APP->pModel->Cfg.BrdConfVector[membrane_brd_row_nomb];

//            brd_nomb_textblock->setText(QString::number(APP->pModel->Cfg.BrdConfVector[membrane_brd_row_nomb]->BrdNomb));
//            APP->pModel->Cfg.BrdConfVector[membrane_brd_row_nomb]->membrane_mode_started = 1;

            // Создаю объект для работы с графиком
            //test_membrane *tm = new test_membrane(this,board_cfg_pointer);
            //graph *gr = new graph(this,board_cfg_pointer);

            //заполняю данные для графика
            //tm->membr_board_row_nomb = membrane_brd_row_nomb;


            // При закрытии дочернего окна надо удалить запись из вектора
            // а при закрытии главного окна удалить все объекты и записи в вектор
            //APP->membrane_wins[board_cfg_pointer] = tm;  // список открытых окон

            //вывожу немодальное окно с графиком
            //tm->show();
//        }
//    }

}

/*--------------------------------------------------------------------*/

void MainWindow::cfg1_Mode()
{
    ch_cfg_mode(1);
    act_Cfg1->setChecked(true);
    act_Cfg2->setChecked(false);
}

/*--------------------------------------------------------------------*/

void MainWindow::cfg2_Mode()
{    
    ch_cfg_mode(2);
    act_Cfg2->setChecked(true);
    act_Cfg1->setChecked(false);
}
/*--------------------------------------------------------------------*/
void MainWindow::ch_cfg_mode(int nomb)
{
    stop_all();
    Sleep(500); // Жду пока все соединения закроются

    delete Tab;
    APP->cfg_nomb = nomb;
    APP->change_cfg();

    // Формирование таблицы списка плат
    Tab = new brd_table(this);
    Tab->resizeColumnsToContents();

    // Добавляю список плат на фрейм
    TableDockWidget->setWidget(Tab);
}

} //namespace SGDK
