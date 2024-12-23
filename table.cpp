#include "table.h"

#include <QAction>

//#include <QList>

#include "application.h"
#include "itemeditdialog.h"

namespace SGDK
{

table::table(QWidget *parent):QTableView(parent)
{
    setProperty("modeName", "Table");

    // Тут представление делается владельцем модели (нетипично)
    // часто владельцем модели является главный объект приложения   =>   setModel(qApp->Model);
    //Model = new BiblioModel(this);  // Тут представление делается владельцем модели (нетипично)
    //setModel(Model); // присоединяет модель к представлению

    setModel( APP->model());    // #define APP dynamic_cast<my_application*>(qApp) - определен в my_application.h

    setContextMenuPolicy( Qt::ActionsContextMenu );
    {
        QAction *A = actEdit = new QAction (this);
        A->setText(tr("Edit"));
        connect(A, SIGNAL(triggered()),this, SLOT(itemEditCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actNew = new QAction (this);
        A->setText(tr("New"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemCreateNew()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }
    {
        QAction *A = actDelete = new QAction (this);
        A->setText(tr("Delete"));
        connect(A, SIGNAL(triggered()),this,SLOT(itemDeleteCurrent()));
        addAction(A); // Присоединяем action к виджету (tableview)
    }

}
table::~table() {}


// слоты для контекстного меню при редактировании эл-та таблицы
void table::itemEditCurrent(void)
{
    // определяем выбранную строку
    if ( ! selectedIndexes().first().isValid())  return;  // ничего не выбрано
    int row = selectedIndexes().first().row();

    DATA::BoardCfg *X = APP->model()->at(row);

    //создаем диалог
    itemEditDialog Dia(this);     //т.к. модальный, то можно его сделать автоматически

    //Присоединяем датасет к диалогу (данные надо присоединить к фрейму, т.к. поля все нах  фрейм инкапсулирован в диалог,
    // то присоединяем к диалогу а затем к фрейму
    Dia.attach(X);
    Dia.exec();
}
void table::itemCreateNew(void)
{

}
void table::itemDeleteCurrent(void)
{

}



} // namespace SGDK
