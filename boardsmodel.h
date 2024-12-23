#ifndef SGDKMODEL_H
#define SGDKMODEL_H

#include <QAbstractTableModel>
//#include <QList>
#include <QColor>

#include "data.h"

namespace SGDK
{

struct gr_color
{
    QString color_style;
    QColor color;
};

class BoardModel : public QAbstractTableModel
{
    Q_OBJECT

  public:

    DATA::config Cfg;

    // цвета графиков и подписей каналов
    gr_color chanel_color[18] = { {"QLabel { color : blue; }",       Qt::blue},
                                  {"QLabel { color : red; }",        Qt::red},
                                  {"QLabel { color : darkYellow; }", Qt::darkYellow},
                                  {"QLabel { color : green; }",      Qt::green},
                                  {"QLabel { color : black; }",      Qt::black},
                                  {"QLabel { color : magenta; }",    Qt::magenta},
                                  {"QLabel { color : gray; }",       Qt::gray},
                                  {"QLabel { color : cyan; }",       Qt::cyan},
                                  {"QLabel { color : darkCyan; }",   Qt::darkCyan},
                                  {"QLabel { color : darkRed; }",       Qt::darkRed},
                                  {"QLabel { color : yellow; }",       Qt::yellow},
                                  {"QLabel { color : yellow; }",       Qt::yellow}
                               };

    void CreateNew(DATA::BoardCfg *X, unsigned int pozition = 64000);
    void Delete(int row);
    void Edit(DATA::BoardCfg *X);

  public:
    BoardModel(QObject *parent = 0);
    virtual ~BoardModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const ;
    int columnCount(const QModelIndex & parent = QModelIndex()) const ;

    void  fillModelFromCfg();

    QColor calculateColorForRow(int row, int col) const ;

    // Переопределяю для отображения заголовков в таблице
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QVariant data( const QModelIndex &I, int role = Qt::DisplayRole) const ;
    DATA::BoardCfg *at(int k) { return Cfg.BrdConfVector.at(k); }
};

}  //namespace SGDK

#endif // SGDKMODEL_H
