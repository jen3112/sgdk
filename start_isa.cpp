#include "start_isa.h"
#include "z_thread.h"
#include "application.h"
//#include <filesystem>

namespace SGDK
{

start_isa::start_isa()
{
}

bool start_isa::active=false;

void start_isa::run()
{
    // Прохожу по всем платам добавленным в вектор активных
    //и смотрю где есть готовые файлы для обработки Исаевской программой

    for (unsigned int i=0; i<APP->pModel->Cfg.BrdConfVector.size(); i++)
    {
        int brd_nomb = APP->pModel->Cfg.BrdConfVector[i]->BrdNomb ;

        //if(z_boards_map_pointer->at(APP->pModel->Cfg.BrdConfVector[i])->isa_file_ready)
        if (APP->pModel->Cfg.BrdConfVector[i]->z_thr.isa_file_ready)
        {
            //z_boards_map_pointer->at(APP->pModel->Cfg.BrdConfVector[i])->isa_file_ready = false;
            APP->pModel->Cfg.BrdConfVector[i]->z_thr.isa_file_ready = false;

            //Перемещаю готовый файл в прогу Исаеву
            QString From_dir = APP->pModel->Cfg.BrdConfVector[i]->isa_prog_dir;
            QString copy_str;
            if(brd_nomb>0)
            {
                copy_str = get_filename_to_copy_isaev(From_dir,2,brd_nomb);
            }
            else
            {
                // если не указан номер платы то использую id
                copy_str = get_filename_to_copy_isaev(From_dir,2,i+1);
            }
            QString IS = APP->pModel->Cfg.BrdConfVector[i]->isa_file_name;

            QFile::rename(IS, copy_str);

            //std::rename(IS.c_str(), copy_str.toUtf8());
            //std::filesystem::rename(IS, copy_str.toStdWString());

            // посылаю сигнал в прогу Исаева
            //int isa_prog_nomb = APP->pModel->Cfg.BrdConfVector[i].isa_prg_nomb;
            //std::wstring mes_str = L"Идентификация и взвешивание составов в движении (весы - " +
            //            std::to_wstring( isa_prog_nomb ) + L")";
            //SendMessage(FindWindow(L"TForm1vi",mes_str.c_str()),WM_USER + 17771,isa_prog_nomb,brd_nomb);
            //SendMessage(FindWindow(L"TForm1vi",mes_str.c_str()),WM_USER + 17771,isa_prog_nomb,i);

            // пауза
            //Sleep(APP->pModel->Cfg.BrdConfVector[n_cfg].isa_pause);
        }

    }
    active = false;
    return;
}

QString start_isa::get_filename_to_copy_isaev(QString path, int weight_prog_num, int weight_num)
{
    QString OutStr = path +
            QString::number(weight_prog_num) +
            "tempS" + QString::number(weight_num) +
            ".dat";
    return OutStr;
}


}  // namespace SGDK

