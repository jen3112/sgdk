#include <QFileDialog>
#include "save_table_csv.h"
#include "application.h"

namespace SGDK
{

void save_string(QTextStream *out, QAbstractItemModel *table_model, int row )
{
    int columns = table_model->columnCount();



    for (int j=0; j<columns; j++)
    {
        QModelIndex index_of_cell =  table_model->index(row-1, j);

        // определяю формат ячейки
        //table_model->data(index_of_cell).

        QString str = table_model->data(index_of_cell).toString();
        *out << str;
        *out << ";";
    }
    *out << "\n";
}

/*------------------------------------------------------------------*/

void save_table_last_str_to_log(QAbstractItemModel *table_model)
{
    QString fileName = APP->pModel->Cfg.archiv_dir + "metrolog.loh";
    int last_row  = table_model->rowCount();
    QFile file;

    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    if (file.isOpen())
    {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Encoding::System);

        save_string(&out, table_model, last_row);

        file.close();
    }
}
/*------------------------------------------------------------------*/


void save_table_csv(QAbstractItemModel *table_model)
{
    // формирую имя файла для записи

    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                QString::fromUtf8("Открыть файл"),
                                QDir::currentPath(),
                                "CSV (*.csv );;All files (*.*)");

    QFile file;
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    int columns = table_model->columnCount();
    int rows  = table_model->rowCount();

    if (file.isOpen())
    {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Encoding::System);

        //  save table headers
        for (int j=0; j<columns; j++)
        {
            QString str = table_model->headerData(j, Qt::Horizontal).toString();

            out << str.toStdWString();
            out << ";";
        }
        out << "\n";


        // save table

        for (int i=0; i<rows; i++)
        { 
            save_string(&out, table_model, i );
        }
        file.close();
    }
    else
    {
        // не могу записать файл
    }
}

}
