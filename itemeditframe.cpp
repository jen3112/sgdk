#include "itemeditframe.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStyle>
#include <QIntValidator>
#include <QString>
#include <QtSql>
#include <QCheckBox>

#include "application.h"
#include "items.h"
#include "itemeditframe.h"
#include "data.h"

namespace SGDK
{

    itemEditFrame::itemEditFrame(QWidget *parent) : QFrame( parent )
    {
        Dt = 0;  // инициализация Dt

        setFrameStyle(StyledPanel | Sunken);
        setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        QVBoxLayout *L = new QVBoxLayout(this);

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          L->addItem(L1);

          setup_BrdNomb(L1);
          setup_Ip(L1);
          setup_Port(L1);
          setup_Side(L1);
          setup_State(L1);
          setup_Process(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);

          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Write bin file : "));
          L1->addWidget(lbl);

          QCheckBox *cbx = cbxBin  = new QCheckBox(this);
          //QLineEdit *txt = edtJournal = new QLineEdit(this);
          L1->addWidget(cbx);

          QLineEdit *txt = edtBin = new QLineEdit(this);
          L1->addWidget(txt);

          L->addItem(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);

          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Write csv file : "));
          L1->addWidget(lbl);

          QCheckBox *cbx = cbxCsv  = new QCheckBox(this);
          //QLineEdit *txt = edtJournal = new QLineEdit(this);
          L1->addWidget(cbx);

          QLineEdit *txt = edtCsv = new QLineEdit(this);
          L1->addWidget(txt);

          L->addItem(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);

          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Write isa file : "));
          L1->addWidget(lbl);

          QCheckBox *cbx = cbxIsa  = new QCheckBox(this);

          L1->addWidget(cbx);

          QLineEdit *txt = edtIsa = new QLineEdit(this);
          L1->addWidget(txt);

          L->addItem(L1);
        }

        setup_wrwheel(L);
        setup_wrzeroes(L);
        setup_isa_real_time(L);

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Isaev files directory : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtIsaProgDir = new QLineEdit(this);
          L1->addWidget(txt);

          L->addItem(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Isaev program nomber : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtIsaProgNomb = new QLineEdit(this);
          txt->setValidator(new QIntValidator(0,24));        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Mode : "));
          L1->addWidget(lbl);

          QComboBox *cmb = combo_box_mode = new QComboBox(this);
          cmb->addItems(QVector<QString>({"test","nonstop","stop"}).toList());

          //txt->setValidator(new QIntValidator(0,24));        // валидация поля
          L1->addWidget(cmb);

          L->addItem(L1);
        }



        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Distance between sensors : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtDatchikDistance = new QLineEdit(this);
          txt->setValidator(new QIntValidator(0,100));        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Power of aproximatin : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtPower = new QLineEdit(this);
          txt->setValidator(new QIntValidator(0,50));        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }
        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Weigth coefficient : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtKvantToWeigth = new QLineEdit(this);
          txt->setValidator(new QDoubleValidator());        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Pause before running Isaev program : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edtIsaPause = new QLineEdit(this);
          txt->setValidator(new QIntValidator(0,50));        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Timeout before reconnect : "));
          L1->addWidget(lbl);

          QLineEdit *txt = edt_max_timeout = new QLineEdit(this);
          txt->setValidator(new QIntValidator(0,50));        // валидация поля
          L1->addWidget(txt);

          L->addItem(L1);
        }


        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          L->addItem(L1);
        }
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_Ip(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        L->addItem(L1);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Ip"));
        L1->addWidget(lbl);

        QLineEdit *txt = edtIp = new QLineEdit(this);
        L1->addWidget(txt);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_Port(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        //L1->SetMinimumSize;

        L->addItem(L1);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Port"));
        lbl->setFixedWidth(40);
        //lbl->setMinimumWidth(50);

        lbl->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        L1->addWidget(lbl);

        QLineEdit *txt = edtPort = new QLineEdit(this);
        txt->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        txt->setFixedWidth(40);
        L1->addWidget(txt);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_Side(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        L->addItem(L1);

        QLabel *lbl = lblSide = new QLabel(this);
        lbl->setFixedWidth(50);
        lbl->setText(tr("Side (0/1)"));
        L1->addWidget(lbl);

        QLineEdit *txt = edtSide = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,1));        // валидация поля
        txt->setFixedWidth(50);
        L1->addWidget(txt);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_BrdNomb(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        L->addItem(L1);

        QLabel *lbl = lblBrdNomb = new QLabel(this);
        lbl->setFixedWidth(40);
        lbl->setText(tr("Nomb"));
        L1->addWidget(lbl);

        QLineEdit *txt = edtBrdNomb = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,24));        // валидация поля
        txt->setFixedWidth(40);
        L1->addWidget(txt);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_State(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        L->addItem(L1);

        QLabel *lbl  = new QLabel(this);

        lbl->setText(tr("State"));
        L1->addWidget(lbl);

        QCheckBox *cbx = cbxState  = new QCheckBox(this);
        //QLineEdit *txt = edtJournal = new QLineEdit(this);
        L1->addWidget(cbx);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_wrwheel(QLayout *L)
    {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);

          QLabel *lbl = new QLabel(this);

          lbl->setText(tr("Write wheels : "));
          L1->addWidget(lbl);

          QCheckBox *cbx = cbx_wrwheel  = new QCheckBox(this);

          L1->addWidget(cbx);

          //QLineEdit *txt = edtIsa = new QLineEdit(this);
          //L1->addWidget(txt);

          L->addItem(L1);

    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_wrzeroes(QLayout *L)
    {

        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Write zeroes : "));
        L1->addWidget(lbl);

        QCheckBox *cbx = cbx_wrzero  = new QCheckBox(this);

        L1->addWidget(cbx);

        //QLineEdit *txt = edtIsa = new QLineEdit(this);
        //L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_isa_real_time(QLayout *L)
    {

        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Process isaev real time : "));
        L1->addWidget(lbl);

        QCheckBox *cbx = cbx_isa_realtime  = new QCheckBox(this);

        L1->addWidget(cbx);

        //QLineEdit *txt = edtIsa = new QLineEdit(this);
        //L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void itemEditFrame::setup_Process(QLayout *L)
    {
        QVBoxLayout *L1 = new QVBoxLayout();
        L1->setSpacing(0);
        //L1->setMargin(0);
        L->addItem(L1);

        QLabel *lbl  = new QLabel(this);

        lbl->setText(tr("Process"));
        L1->addWidget(lbl);

        QCheckBox *cbx = cbxProcess  = new QCheckBox(this);
        L1->addWidget(cbx);
    }

/* --------------------------------------------------------- */

    itemEditFrame::~itemEditFrame()   {   }

/* --------------------------------------------------------- */

    void itemEditFrame::attach ( DATA::BoardCfg *X )
    {
        Dt = X;
        if( ! Dt) return;

        // общая часть
        edtPort ->setText(QString::number(X->port));
        edtBrdNomb->setText( QString::number(X->BrdNomb ) );
        edtIp->setText(QString::number(X->ip[0]) + "." + QString::number(X->ip[1])+ "." +
                       QString::number(X->ip[2]) + "." + QString::number(X->ip[3]));

        cbxState->setCheckState( (X->State) ? Qt::Checked : Qt::Unchecked );
        cbxProcess->setCheckState( (X->process) ? Qt::Checked : Qt::Unchecked );

        cbxIsa->setCheckState( (X->write_isa) ? Qt::Checked : Qt::Unchecked );
        cbxBin->setCheckState( (X->write_bin) ? Qt::Checked : Qt::Unchecked );
        cbxCsv->setCheckState( (X->write_csv) ? Qt::Checked : Qt::Unchecked );

        edtIsa->setText( X->isa_file_name );
        edtBin->setText(X->bin_file_name);
        edtCsv->setText(X->csv_file_name);

        cbx_wrwheel->setCheckState( (X->write_wheel) ? Qt::Checked : Qt::Unchecked );
        cbx_wrzero->setCheckState( (X->write_zero) ? Qt::Checked : Qt::Unchecked );

        cbx_isa_realtime->setCheckState( (X->isa_real_time) ? Qt::Checked : Qt::Unchecked );

        edt_max_timeout->setText(QString::number(X->max_timeout));

        combo_box_mode->setCurrentIndex(X->BoardMode);

        edtDatchikDistance->setText( QString::number(X->datchik_distance) );
        edtIsaProgNomb->setText( QString::number(X->isa_prg_nomb) );
        edtIsaProgDir->setText(X->isa_prog_dir);

        edtPower->setText( QString::number(X->power) );
        edtSide->setText(  QString::number(X->side)  );

        edtKvantToWeigth->setText( QString::number(X->kvant_to_weigth) );
        edtIsaPause->setText( QString::number(X->isa_pause) );

    }

    void itemEditFrame::save ( void )   // должна снимать данные с фрейма и сохранять в структуре
    {

        bool ok;
        Dt->BrdNomb  = edtBrdNomb->text().toInt(&ok,10);
        Dt->port = edtPort->text().toInt(&ok,10);

        DATA::splitIp(edtIp->text().toStdWString() , Dt->ip);

        if (cbxState->checkState() == Qt::Checked)
        {
            if( Dt->State ==  SGDK::DATA::BoardState::Off )
            {
                // если статус другой то не меняю
                Dt->State = SGDK::DATA::BoardState::On ;
            }
        }
        else
        {
            Dt->State = SGDK::DATA::BoardState::Off ;
        }

        Dt->datchik_distance = edtDatchikDistance->text().toInt(&ok,10);

        Dt->process   = (cbxProcess->checkState() == Qt::Checked)?true:false;
        Dt->write_bin = (cbxBin    ->checkState() == Qt::Checked)?true:false;
        Dt->write_csv = (cbxCsv    ->checkState() == Qt::Checked)?true:false;
        Dt->write_isa = (cbxIsa    ->checkState() == Qt::Checked)?true:false;
        Dt->write_wheel=(cbx_wrwheel->checkState() == Qt::Checked)?true:false;
        Dt->write_zero =(cbx_wrzero ->checkState() == Qt::Checked)?true:false;
        Dt->isa_real_time=(cbx_isa_realtime->checkState() == Qt::Checked)?true:false;

        Dt->max_timeout = edt_max_timeout->text().toInt(&ok,10);

        if (combo_box_mode->currentIndex() == 0)
        {
            Dt->BoardMode = SGDK::DATA::BrdModes::test;
        }
        else if(combo_box_mode->currentIndex() == 1)
        {
            Dt->BoardMode = SGDK::DATA::BrdModes::nonstop;
        }
        else
        {
            Dt->BoardMode = SGDK::DATA::BrdModes::stop;
        }

        Dt->bin_file_name = edtBin->text();
        Dt->csv_file_name = edtCsv->text();
        Dt->isa_file_name = edtIsa->text();

        Dt->isa_prg_nomb = edtIsaProgNomb->text().toInt(&ok,10);
        Dt->isa_prog_dir = edtIsaProgDir->text();
        Dt->power = edtPower->text().toInt(&ok,10);

        Dt->side  = edtSide->text().toInt(&ok,10);
        Dt->kvant_to_weigth =edtKvantToWeigth->text().toDouble(&ok);

        Dt->isa_pause = edtIsaPause->text().toInt(&ok,10);

        // сохраняю настройки всех плат в файл
        //APP->save_config();
    }

    bool itemEditFrame::isValid() const  // проверка валидности данных
    {
        if(edtPort->text().size()<=0) return true;
        return edtPort->text().at(0) != '.' ;    // Проверка что title не начинается с .
    }



}   // namespace SGDK
