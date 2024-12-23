#ifndef MAINWINDOWSTATE_H
#define MAINWINDOWSTATE_H

#include <QPoint>
#include <QSize>


class MainWindowState
{
public:
    explicit MainWindowState();
    void SaveState(const QPoint &pos, const QSize &size);
    void RestoreState(QPoint &pos, QSize &size);



};

#endif // MAINWINDOWSTATE_H
