#ifndef SAVE_TABLE_CSV_H
#define SAVE_TABLE_CSV_H

#include <QTableView>
#include <QTextStream>
#include <QFile>

namespace SGDK
{
    void save_string(QTextStream *out, QAbstractItemModel *table_model, int row );
    void save_table_csv(QAbstractItemModel *table_model);
    void save_table_last_str_to_log(QAbstractItemModel *table_model);
}
#endif // SAVE_TABLE_CSV_H
