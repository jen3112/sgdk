#ifndef BRD_TABLE_H
#define BRD_TABLE_H

#include <QTableView>
#include <QFile>

class QAction;


namespace SGDK
{

class brd_table : public QTableView
{
    Q_OBJECT

  private:
    QAction *actOn ;
    QAction *actOff ;
    QAction *actEdit ;
    QAction *actNew  ;
    QAction *actDelete;

    QAction *actUp;
    QAction *actDown;

    QAction *actTestMode;
    QAction *actStopMode;
    QAction *actNonstopMode;

    QAction *actWriteBinMode;
    QAction *actWriteCsvMode;

    QAction *separator0;
    QAction *separator1;
    QAction *separator2;

  public:
    brd_table(QWidget *parent = 0);
    virtual ~brd_table();

    void update_values(); // перерисовка таблицы при обновлении данных

  protected slots:
    // слоты для контекстного меню при редактировании эл-та таблицы
    void itemOnCurrent(void);
    void itemOffCurrent(void);
    void itemEditCurrent(void);
    void itemCopyCurrent(void);
    void itemCreateNew(void);
    void itemDeleteCurrent(void);

    void itemUpCurrent(void);
    void itemDownCurrent(void);

    void itemSetTestMode(void);
    void itemSetStopMode(void);
    void itemSetNonstopMode(void);
    void itemSetWriteBin(void);
    void itemSetWriteCsv(void);

};

} // namespace SGDK

#endif // BRD_TABLE_H
