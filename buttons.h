#ifndef BUTTONS_H
#define BUTTONS_H

#include <QFrame>
//#include <qcustomplot.h>
#include <QDebug>

class QPushButton;

namespace SGDK
{


// Вспомогательный класс с кнопками
// Этот класс целиком потом кладем на диалог
class Buttons : public QFrame
{
    Q_OBJECT
  private:
    QPushButton *pOK;
    QPushButton *pCancel;
  public:
    Buttons(QWidget *parent = 0);

  signals:
    void accepting(void);
    void rejecting(void);

};


}


#endif // BUTTONS_H
