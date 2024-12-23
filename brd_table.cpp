#include "brd_table.h"

#include <QAction>
#include <QFile>

//#include <QList>
#include "time_str.h"
#include "application.h"
#include "itemeditdialog.h"

namespace SGDK
{

void brd_table::update_values()
{
    setUpdatesEnabled(false);
    verticalHeader()->hide();
    verticalHeader()->show();
    setUpdatesEnabled(true);
    resizeColumnsToContents();
}

/* --------------------------------------------------- */

brd_table::brd_table(QWidget *parent):QTableView(parent)
{
    setProperty("modeName", "Table");

    // Тут представление делается владельцем модели (нетипично)
    // часто владельцем модели является главный объект приложения   =>   setModel(qApp->Model);
    //Model = new BiblioModel(this);  // Тут представление делается владельцем модели (нетипично)
    //setModel(Model); // присоединяет модель к представлению

    setModel( APP->model());    // #define APP dynamic_cast<my_application*>(qApp) - определен в my_application.h

    setContextMenuPolicy( Qt::ActionsContextMenu );
    {
        QAction *A = actOn = new QAction (this);
        A->setText(tr("On"));
        connect(A, SIGNAL(triggered()),this, SLOT(itemOnCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actOff = new QAction (this);
        A->setText(tr("Off"));
        connect(A, SIGNAL(triggered()),this, SLOT(itemOffCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actEdit = new QAction (this);
        A->setText(tr("Edit"));
        connect(A, SIGNAL(triggered()),this, SLOT(itemEditCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actEdit = new QAction (this);
        A->setText(tr("Copy"));
        connect(A, SIGNAL(triggered()),this, SLOT(itemCopyCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actNew = new QAction (this);
        A->setText(tr("New"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemCreateNew()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actDelete = new QAction (this);
        A->setText(tr("Delete"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemDeleteCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }

    // разделитель в контекстном меню
    separator0 = new QAction(this);
    separator0->setSeparator(true);
    addAction(separator0);

    {
        QAction *A = actUp = new QAction (this);
        A->setText(tr("Move Up"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemUpCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }

    {
        QAction *A = actDown = new QAction (this);
        A->setText(tr("Move Down"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemDownCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }


    // разделитель в контекстном меню
     separator1 = new QAction(this);
    separator1->setSeparator(true);
    addAction(separator1);

    {
        QAction *A = actTestMode = new QAction (this);
        A->setText(tr("Test mode"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemSetTestMode()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actStopMode = new QAction (this);
        A->setText(tr("Stop mode"));
        //A->setCheckable(true);
        connect(A, SIGNAL(triggered()),this,SLOT(itemSetStopMode()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actNonstopMode = new QAction (this);
        A->setText(tr("Nonstop mode"));
        //A->setCheckable(true);
        connect(A, SIGNAL(triggered()),this,SLOT(itemSetNonstopMode()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }

    // разделитель в контекстном меню
    separator2 = new QAction(this);
    separator2->setSeparator(true);
    addAction(separator2);

    {
        QAction *A = actWriteBinMode = new QAction (this);
        A->setText(tr("Write BIN"));
        //A->setCheckable(true);
        connect(A, SIGNAL(triggered()),this,SLOT(itemSetWriteBin()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actWriteCsvMode = new QAction (this);
        A->setText(tr("Write CSV"));
        //A->setCheckable(true);
        connect(A, SIGNAL(triggered()),this,SLOT(itemSetWriteCsv()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }

}

/* --------------------------------------------------- */

brd_table::~brd_table()
{
    // если удалять акшены то прога не закрывается ???
    delete actOn ;
    delete actOff ;
    delete actEdit ;
    delete actNew  ;
    delete actDelete;

    delete actUp;
    delete actDown;

    delete actTestMode;
    delete actStopMode;
    delete actNonstopMode;

    delete actWriteBinMode;
    delete actWriteCsvMode;

    delete separator0;
    delete separator1;
    delete separator2;

}

/* --------------------------------------------------- */

// слоты для контекстного меню при редактировании эл-та таблицы

void brd_table::itemSetWriteBin(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    if (APP->pModel->Cfg.BrdConfVector[row]->write_custom_bin)
    {
        // закрываю файл bin для записи
        APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file->close();

        //удаляю объект QFile*
        delete APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file;

        APP->pModel->Cfg.BrdConfVector[row]->write_custom_bin = false;
    }
    else
    {

        // открываю новый bin файл для записи
        APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file_name =
                                APP->pModel->Cfg.archiv_dir + "\\"
                                + "cus_bin\\brd_"
                                + QString::number(APP->pModel->Cfg.BrdConfVector[row]->BrdNomb)
                                + "-" + get_time_str()  + ".bin";
        APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file = new QFile();

        APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file->setFileName(APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file_name);

        /*bool OK = */APP->pModel->Cfg.BrdConfVector[row]->custom_bin_file->open(QIODevice::WriteOnly);

        APP->pModel->Cfg.BrdConfVector[row]->write_custom_bin = true;
    }
}

/* --------------------------------------------------- */

void brd_table::itemSetWriteCsv(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    if (APP->pModel->Cfg.BrdConfVector[row]->write_custom_csv)
    {
        // закрываю файл bin для записи
        APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file->close();

        //удаляю объект QFile*
        delete APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file;

        APP->pModel->Cfg.BrdConfVector[row]->write_custom_csv = false;
    }
    else
    {

        // открываю новый bin файл для записи
        APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file_name =
                                APP->pModel->Cfg.archiv_dir + "\\"
                                + "cus_csv\\brd_"
                                + QString::number(APP->pModel->Cfg.BrdConfVector[row]->BrdNomb)
                                + "-" + get_time_str() + ".csv";
        APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file = new QFile();

        APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file->setFileName(APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file_name);

        /*bool OK =*/ APP->pModel->Cfg.BrdConfVector[row]->custom_csv_file->open(QIODevice::WriteOnly);

        APP->pModel->Cfg.BrdConfVector[row]->write_custom_csv = true;
    }
}

/* --------------------------------------------------- */

void brd_table::itemOnCurrent(void)
{
    QModelIndexList index_list = selectedIndexes();
    QModelIndex index;

    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано

    foreach(index, index_list)
    {
        int row = index.row();  // определяем выбранную строку
        if ( (APP->model()->at(row)->State != SGDK::DATA::Connected) &&
             (APP->model()->at(row)->State != SGDK::DATA::Train_detection) )
        {
        /*DATA::BoardCfg *X =*/ APP->model()->at(row)->State = SGDK::DATA::On;
        }
    }


    //X->State = SGDK::DATA::On;
}

/* --------------------------------------------------- */

void brd_table::itemOffCurrent(void)
{
    QModelIndexList index_list = selectedIndexes();
    QModelIndex index;

    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано

    foreach(index, index_list)
    {
        int row = index.row();  // определяем выбранную строку

        /*DATA::BoardCfg *X =*/ APP->model()->at(row)->State = SGDK::DATA::Off;
    }


}

/* --------------------------------------------------- */

void brd_table::itemEditCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    DATA::BoardCfg *X = APP->model()->at(row);

    if (X->State == SGDK::DATA::Off)
    {

        //создаем диалог
        itemEditDialog Dia(this);     //т.к. модальный, то можно его сделать автоматически

        //Присоединяем датасет к диалогу (данные надо присоединить к фрейму, т.к. поля все нах  фрейм инкапсулирован в диалог,
        // то присоединяем к диалогу а затем к фрейму
        Dia.attach(X);

        if (Dia.exec() == QDialog::Accepted)
        {
            APP->model()->Edit(X);  // запись в БД
        }
    }
}

/* --------------------------------------------------- */

void brd_table::itemCopyCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    DATA::BoardCfg *X = APP->model()->at(row);
    DATA::BoardCfg *Y = new DATA::BoardCfg(*X);

    Y->State = DATA::Off;

    APP->model()->CreateNew(Y,row);
}

/* --------------------------------------------------- */

void brd_table:: itemUpCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    unsigned int row = selectedIndexes().first().row();

    if (row == 0) return;

    DATA::BoardCfg *X = APP->model()->at(row);
    DATA::BoardCfg *Y = APP->model()->at(row-1);

    APP->model()->Cfg.BrdConfVector[row] = Y;
    APP->model()->Cfg.BrdConfVector[row-1] = X;

    APP->save_config();
}

/* --------------------------------------------------- */

void brd_table:: itemDownCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    unsigned int row = selectedIndexes().first().row();

    if (row == APP->model()->Cfg.BrdConfVector.size()-1) return;

    DATA::BoardCfg *X = APP->model()->at(row);
    DATA::BoardCfg *Y = APP->model()->at(row+1);

    APP->model()->Cfg.BrdConfVector[row] = Y;
    APP->model()->Cfg.BrdConfVector[row+1] = X;

    APP->save_config();
}

/* --------------------------------------------------- */

void brd_table::itemCreateNew(void)
{
    // если выход из диалога по cancell, то надо удалить X
    DATA::BoardCfg *X = new DATA::BoardCfg();

    //создаем диалог
    itemEditDialog Dia(this);     //т.к. модальный, то можно его сделать автоматически

    //Присоединяем датасет к диалогу (данные надо присоединить к фрейму, т.к. поля все нах  фрейм инкапсулирован в диалог,
    //то присоединяем к диалогу а затем к фрейму
    Dia.attach(X);

    if (Dia.exec() == QDialog::Accepted)
    {

        APP->model()->CreateNew(X);
    }
    else
    {
        delete X;
    }
}

/* --------------------------------------------------- */

void brd_table::itemDeleteCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())   return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    // заканчиваю связанные процессы
    APP->model()->Cfg.BrdConfVector[row]->State = SGDK::DATA::Off;

    //закрываю окно графика
    if (APP->model()->Cfg.BrdConfVector[row]->gr_win.graph_win_is_open == true)
    {
        APP->model()->Cfg.BrdConfVector[row]->gr_win.close();
    }
    APP->model()->Delete(row);

    // сохраняю конфиг
    APP->save_config();
}

/*--------------------------------------------------------------*/

// Слоты выбора режима работы платы

/*--------------------------------------------------------------*/

void brd_table::itemSetTestMode(void)
{
    if ( ! selectedIndexes().first().isValid()) return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    actTestMode->setChecked(true);
    actStopMode->setChecked(false);
    actNonstopMode->setChecked(false);

    /*DATA::BoardCfg *X =*/ APP->model()->at(row)->BoardMode = SGDK::DATA::BrdModes::test;
    //X->BoardMode = SGDK::DATA::BrdModes::test;
}

/*--------------------------------------------------------------*/

void brd_table::itemSetStopMode(void)
{
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    actTestMode->setChecked(false);
    actStopMode->setChecked(true);
    actNonstopMode->setChecked(false);

    /*DATA::BoardCfg *X =*/ APP->model()->at(row)->BoardMode = SGDK::DATA::BrdModes::stop;
    //X->BoardMode = SGDK::DATA::BrdModes::stop;
}

/*--------------------------------------------------------------*/

void brd_table::itemSetNonstopMode(void)
{
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    actTestMode->setChecked(false);
    actStopMode->setChecked(false);
    actNonstopMode->setChecked(true);

    /*DATA::BoardCfg *X =*/ APP->model()->at(row)->BoardMode = SGDK::DATA::BrdModes::nonstop;
    //X->BoardMode = SGDK::DATA::BrdModes::nonstop;
}

/*--------------------------------------------------------------*/


} // namespace SGDK
