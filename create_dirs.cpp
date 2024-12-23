#include <QString>
#include <QDir>
#include "create_dirs.h"

namespace SGDK
{

void create_dir(QString path)
{
    QDir dir(path);
    if (!dir.exists())
    {
      dir.mkdir(".");
    }
}

void create_dirs(QString root_archiv_path)
{
    //APP->pModel->Cfg.archiv_dir
    //APP->pModel->Cfg.archive_folder = APP->pModel->Cfg.archiv_dir + get_time_str() ;
    create_dir(root_archiv_path);
    create_dir(root_archiv_path + + "\\cus_bin");
    create_dir(root_archiv_path + + "\\cus_csv");
    create_dir(root_archiv_path + + "\\graph");
}

}   // namespace SGDK
