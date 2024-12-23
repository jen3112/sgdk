#ifndef CREATE_DIRS_H
#define CREATE_DIRS_H

#include <QString>

// Создаем структуру папок, необходимую для записи bin, csv файлов

namespace SGDK
{
    void create_dir(QString path);
    void create_dirs(QString root_archiv_path);

}  // namespace SGDK

#endif // CREATE_DIRS_H
