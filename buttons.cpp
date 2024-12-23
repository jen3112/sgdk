#include "buttons.h"

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCloseEvent>

namespace SGDK
{

Buttons::Buttons(QWidget *parent) // конструктор вспомогательного класса
:QFrame(parent)
{
    // стиль фрейма
    setFrameStyle(StyledPanel | Sunken);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

    // после создания фрейма прикручиваем к нему лэйаут
    // создаем разметку внутри фрейма
    QHBoxLayout *L = new QHBoxLayout(this);

    // Добавляем пустой эл-т чтобы сдвинуть кнопки
    L->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding,
                                    QSizePolicy::Minimum));
    {
      QPushButton *B = pOK = new QPushButton(this);
      // Добавляем кнопку на лэйаут
      L->addWidget(B);
      B->setText(tr("OK"));
      connect(B,SIGNAL(clicked()),this, SIGNAL(accepting()));
    }
    {
      QPushButton *B = pCancel = new QPushButton(this);
      L->addWidget(B);
      B->setText(tr("CANCEL"));
      connect(B,SIGNAL(clicked()),this, SIGNAL(rejecting()));
    }
}
}
