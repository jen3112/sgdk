#include <QSettings>

#include "mainwindowstate.h"

MainWindowState::MainWindowState()
{
}

void MainWindowState::SaveState(const QPoint &pos, const QSize &size)
{
    QSettings settings("AltuninVV", "PosSizeDemo");
    settings.beginGroup("MainWindow");
        settings.setValue("pos", pos);
        settings.setValue("size", size);
    settings.endGroup();
}

void MainWindowState::RestoreState(QPoint &pos, QSize &size)
{
    QSettings settings("AltuninVV", "PosSizeDemo");
    settings.beginGroup("MainWindow");
        QPoint oldPoint = settings.value("pos", QPoint(0, 0)).toPoint();
        if (oldPoint.x()!=0 && oldPoint.y()!=0) //check if there old values
        {
            pos.setX(oldPoint.x());
            pos.setY(oldPoint.y());
        }

        QSize oldSize = settings.value("size", QSize(0, 0)).toSize();
        if (oldSize.width()!=0 && oldSize.height()!=0) //check if there old values
        {
            size.setWidth(oldSize.width());
            size.setHeight(oldSize.height());
        }

        settings.endGroup();
}
