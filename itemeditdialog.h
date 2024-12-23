#ifndef ITEMEDITDIALOG_H
#define ITEMEDITDIALOG_H

#include <QDialog>
#include <QFrame>
//#include <qcustomplot.h>
#include <QDebug>

#include "itemeditframe.h"
#include "data.h"
#include "buttons.h"


namespace SGDK
{





class itemEditDialog : public QDialog
{
    Q_OBJECT


private:
    //graphFrame *grFrame;  // frame to put graphic

    //QCustomPlot *wGraphic;      // Объявляем объект QCustomPlot
   // QCPCurve *verticalLine;     // Объявляем объект для вертикальной линии
   // QCPItemTracer *tracer;      // Трасировщик по точкам графика

    itemEditFrame *Edt;
    Buttons * Btn;

public:
    itemEditDialog( QWidget *parent=0);

    virtual ~itemEditDialog();

    void attach(DATA::BoardCfg * X) { Edt->attach(X);}
protected:
    void closeEvent (QCloseEvent *E);    //



protected slots:
    void accept();
    void reject();
};

}  // namespace SGDK

#endif // ITEMEDITDIALOG_H
