#ifndef TABLE_H
#define TABLE_H

#include <QTableView>

class QAction;

//#include "bibliomodel.h"

namespace SGDK
{

class table : public QTableView
{
    Q_OBJECT

  private:
    QAction *actEdit ;
    QAction *actNew  ;
    QAction *actDelete;

//  private:
//    BiblioModel *pModel ;           // модель получается инкапсулирована в представление, это распространенный но нетипичный прием
  public:
    table(QWidget *parent = 0);
    virtual ~table();

  protected slots:
    // слоты для контекстного меню при редактировании эл-та таблицы
    void itemEditCurrent(void);
    void itemCreateNew(void);
    void itemDeleteCurrent(void);
};

} // namespace SGDK

#endif // TABLE_H
