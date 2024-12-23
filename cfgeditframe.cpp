#include "cfgeditframe.h"

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
//#include "itemeditframe.h"
#include "data.h"

namespace SGDK
{

    void cfgEditFrame::setup_arch_dir (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Archiv dir : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_arch_dir = new QLineEdit(this);
        //txt->setValidator(new QRegularExpressionValidator());        // валидация поля
        L1->addWidget(txt);
        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_update_timer_interval_normal (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("Update table interval : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_update_timer_interval_normal = new QLineEdit(this);
        txt->setValidator(new QIntValidator(400,50000));        // валидация поля
        L1->addWidget(txt);
        L->addItem(L1);
    }

    /* --------------------------------------------------------- */

        void cfgEditFrame::setup_update_timer_interval_max (QLayout *L)
        {
            QHBoxLayout *L1 = new QHBoxLayout();
            //L1->setMargin(0);

            QLabel *lbl = new QLabel(this);

            lbl->setText(tr("Update table interval max: "));
            L1->addWidget(lbl);

            QLineEdit *txt = edt_update_timer_interval_max = new QLineEdit(this);
            txt->setValidator(new QIntValidator(400,50000));        // валидация поля
            L1->addWidget(txt);
            L->addItem(L1);
        }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_max_arhive_nomb (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("max arhive nomb : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_max_arhive_nomb = new QLineEdit(this);
        txt->setValidator(new QIntValidator());        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_etalon_com_port_nomb (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("etalon com port nomb : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_etalon_com_port_nomb = new QLineEdit(this);
        //txt->setValidator(new QIntValidator(0,30));        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_min_dac_value (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("min dac value : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_min_dac_value = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,3000));        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_max_dac_value (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("max dac value : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_max_dac_value = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,3000));        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::setup_recomended_min_dac_value (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("recomended min dac value : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_recomended_min_dac_value = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,30));        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }


/* --------------------------------------------------------- */

    void cfgEditFrame::setup_recomended_max_dac_value (QLayout *L)
    {
        QHBoxLayout *L1 = new QHBoxLayout();
        //L1->setMargin(0);

        QLabel *lbl = new QLabel(this);

        lbl->setText(tr("recomended_max dac value : "));
        L1->addWidget(lbl);

        QLineEdit *txt = edt_recomended_max_dac_value = new QLineEdit(this);
        txt->setValidator(new QIntValidator(0,30));        // валидация поля
        L1->addWidget(txt);

        L->addItem(L1);
    }

/* --------------------------------------------------------- */

    cfgEditFrame::cfgEditFrame(QWidget *parent) : QFrame( parent )
    {
        //Dt = 0;  // инициализация Dt

        setFrameStyle(StyledPanel | Sunken);
        setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        QVBoxLayout *L = new QVBoxLayout(this);

        setup_arch_dir (L);
        setup_max_arhive_nomb(L);
        setup_etalon_com_port_nomb(L);

        setup_min_dac_value(L);
        setup_max_dac_value(L);
        setup_recomended_min_dac_value(L);
        setup_recomended_max_dac_value(L);
        setup_update_timer_interval_normal(L);
        setup_update_timer_interval_max(L);

        {
          QHBoxLayout *L1 = new QHBoxLayout();
          //L1->setMargin(0);
          L->addItem(L1);
        }
    }


/* --------------------------------------------------------- */

    cfgEditFrame::~cfgEditFrame()   {   }

/* --------------------------------------------------------- */

    void cfgEditFrame::attach (  )
    {
        edt_arch_dir->setText(APP->pModel->Cfg.archiv_dir);
        edt_max_arhive_nomb->setText(QString::number(APP->pModel->Cfg.max_arhive_nomb));
        edt_etalon_com_port_nomb->setText(QString::number(APP->pModel->Cfg.etalon_com_port_name));
        edt_min_dac_value->setText(QString::number(APP->pModel->Cfg.Min_Dac_value));
        edt_max_dac_value->setText(QString::number(APP->pModel->Cfg.Max_Dac_value));
        edt_recomended_min_dac_value->setText(QString::number(APP->pModel->Cfg.Recomended_Min_Dac_value));
        edt_recomended_max_dac_value->setText(QString::number(APP->pModel->Cfg.Recomended_Max_Dac_value));
        edt_update_timer_interval_normal->setText(QString::number(APP->pModel->Cfg.update_timer_interval_normal));
        edt_update_timer_interval_max->setText(QString::number(APP->pModel->Cfg.update_timer_interval_max));
    }

/* --------------------------------------------------------- */

    void cfgEditFrame::save ( void )   // должна снимать данные с фрейма и сохранять в структуре
    {
        bool ok;


        APP->pModel->Cfg.archiv_dir = edt_arch_dir->text();
        APP->pModel->Cfg.max_arhive_nomb = edt_max_arhive_nomb->text().toInt(&ok,10);
        APP->pModel->Cfg.etalon_com_port_name = edt_etalon_com_port_nomb->text().toInt(&ok,10);
        APP->pModel->Cfg.Min_Dac_value = edt_min_dac_value->text().toInt(&ok,10);
        APP->pModel->Cfg.Max_Dac_value = edt_max_dac_value->text().toInt(&ok,10);
        APP->pModel->Cfg.Recomended_Min_Dac_value = edt_recomended_min_dac_value->text().toInt(&ok,10);
        APP->pModel->Cfg.Recomended_Max_Dac_value = edt_recomended_max_dac_value->text().toInt(&ok,10);
        APP->pModel->Cfg.update_timer_interval_normal = edt_update_timer_interval_normal->text().toInt(&ok,10);
        APP->pModel->Cfg.update_timer_interval_max = edt_update_timer_interval_max->text().toInt(&ok,10);



        //Dt->kvant_to_weigth = edtKvantToWeigth->text().toDouble(&ok);

        //Dt->isa_pause = edtIsaPause->text().toInt(&ok,10);

        // сохраняю настройки всех плат в файл
        //APP->save_config();
    }

/* --------------------------------------------------------- */

    bool cfgEditFrame::isValid() const  // проверка валидности данных
    {
        //if(edtPort->text().size()<=0) return true;
        //return edtPort->text().at(0) != '.' ;    // Проверка что title не начинается с .

        return true;
    }



}   // namespace SGDK
