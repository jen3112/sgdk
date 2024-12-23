#include "cfgeditdialog.h"

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCloseEvent>

namespace SGDK
{

cfgEditDialog::cfgEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle( tr("Editing application config") ); // заголовок диалога

    // создаем разметку диалога(this)
    QVBoxLayout *L0 = new QVBoxLayout(this);

    // Добавочный верхний фрейм
    {
      Edt = new cfgEditFrame(this);
      L0->addWidget(Edt);
    }

    // фрейм с кнопками (нижний)
    {
      Btn = new Buttons(this); // создаем фрейм
      L0->addWidget(Btn);
      connect(Btn, SIGNAL(accepting()),this, SLOT(accept()));
      connect(Btn, SIGNAL(rejecting()),this, SLOT(reject()));
    }
}

/*------------------------------------------------------------------*/

cfgEditDialog::~cfgEditDialog()
{
}

void cfgEditDialog::closeEvent(QCloseEvent *E)  // чтоб ей пользоваться надо подменить фильтр
{
/*    switch (result())
    {
        case Rejected:          // пользователь нажал на Cancel
        {
            // надо удалить объект X если он задан в пункте New


            E->accept();
            break;
        }
        case Accepted:
        {
            if(Edt->isValid())
            {
                Edt->save();     // надо написать ф-ю save
                E->accept();
            }
            else {E->ignore();}
        }
        default:
        {
            E->accept();
            break;
        }
    }
    */

    QDialog::closeEvent(E);
}

void cfgEditDialog::accept()
{
    if (Edt->isValid())
    {
        Edt->save();
        QDialog::accept();
    }
}

void cfgEditDialog::reject()
{
    QDialog::reject(); //выход без сохранения
}

} // namespace SGDK
