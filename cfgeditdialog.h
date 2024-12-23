#ifndef CFGEDITDIALOG_H
#define CFGEDITDIALOG_H

#include <QDialog>
#include <QFrame>
//#include <qcustomplot.h>
#include <QDebug>

#include "cfgeditframe.h"
#include "data.h"
#include "buttons.h"


namespace SGDK
{





class cfgEditDialog : public QDialog
{
    Q_OBJECT


private:
    //graphFrame *grFrame;  // frame to put graphic

    //QCustomPlot *wGraphic;      // Объявляем объект QCustomPlot
   // QCPCurve *verticalLine;     // Объявляем объект для вертикальной линии
   // QCPItemTracer *tracer;      // Трасировщик по точкам графика

    cfgEditFrame *Edt;
    Buttons * Btn;

public:
    cfgEditDialog( QWidget *parent=0);

    virtual ~cfgEditDialog();

    void attach() { Edt->attach();}
protected:
    void closeEvent (QCloseEvent *E);    //



protected slots:
    void accept();
    void reject();
};

}  // namespace SGDK

#endif // ITEMEDITDIALOG_H
