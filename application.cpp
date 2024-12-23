#include "application.h"
#include <QtSql/QSqlDatabase>

#include "data.h"

namespace SGDK {

application::application(int argc, char* argv[])
    : QApplication(argc, argv)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5433);
    db.setDatabaseName("sgdk");
    db.setUserName("admin");
    db.setPassword("1");
    db.open("admin","1");

    set_cfg();
    /*pModel = new BoardModel(this);  // так модель инкапсулируется в Application
    //int I = 5;
    QVariant E = QVariant(5);
    pModel->setHeaderData(1,Qt::Horizontal,E,Qt::DisplayRole);
    QVariant L = pModel->headerData(1,Qt::Horizontal,Qt::DisplayRole);

    pModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));*/
}

void application::change_cfg()
{
    delete pModel;
    set_cfg();
}

QString application::cfg_file_name()
{
    QString S = QString::number(cfg_nomb) + ".cfg";
    return S;
}

void application::set_cfg()
{
    pModel = new BoardModel(this);

    QVariant E = QVariant(5);
    pModel->setHeaderData(1,Qt::Horizontal,E,Qt::DisplayRole);
    QVariant L = pModel->headerData(1,Qt::Horizontal,Qt::DisplayRole);

    pModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
}

void application::save_config()
{


    std::wofstream CfgFile (cfg_file_name().toStdString());
    if (CfgFile.is_open())
    {
        //сохраняю настройки приложения
        CfgFile << ":app" << std::endl;
        CfgFile << "menu_pos:left" << std::endl;
        //CfgFile << "archiv_dir:c:\\archiv\\" << std::endl;
        CfgFile << "archiv_dir:" << APP->pModel->Cfg.archiv_dir.toStdWString() << std::endl;

        //QString::fromStdWString
        CfgFile << "max_arhive_nomb:" << APP->pModel->Cfg.max_arhive_nomb << std::endl;
        CfgFile << "etalon_com_port_name:" << APP->pModel->Cfg.etalon_com_port_name << std::endl;

        CfgFile << "min_dac_value:" << APP->pModel->Cfg.Min_Dac_value  << std::endl;
        CfgFile << "max_dac_value:" << APP->pModel->Cfg.Max_Dac_value  << std::endl;
        CfgFile << "recomended_min_dac_value:" << APP->pModel->Cfg.Recomended_Min_Dac_value  << std::endl;
        CfgFile << "recomended_max_dac_value:" << APP->pModel->Cfg.Recomended_Max_Dac_value  << std::endl;
        //int tmr_int = APP->pModel->Cfg.update_timer_interval;
        CfgFile << "update_timer_interval_normal:" << APP->pModel->Cfg.update_timer_interval_normal  << std::endl;
        CfgFile << "update_timer_interval_max:" << APP->pModel->Cfg.update_timer_interval_max  << std::endl;


        CfgFile << std::endl;

        //сохраняю настройки платы
        for (int id=0; id<APP->pModel->Cfg.BrdConfVector.size(); id++)
        {
            //CfgFile << ":brd:" << APP->pModel->Cfg.BrdConfVector[id].BrdNomb << std::endl;
            CfgFile << ":brd:" << id << std::endl;
            CfgFile << "ip:" << APP->pModel->Cfg.BrdConfVector[id]->ip[0] << "."
                             << APP->pModel->Cfg.BrdConfVector[id]->ip[1] << "."
                             << APP->pModel->Cfg.BrdConfVector[id]->ip[2] << "."
                             << APP->pModel->Cfg.BrdConfVector[id]->ip[3] << std::endl;
            CfgFile << "port:" << APP->pModel->Cfg.BrdConfVector[id]->port << std::endl;

            if (APP->pModel->Cfg.BrdConfVector[id]->BoardMode == DATA::BrdModes::test)
            {
                CfgFile << "mode:test" << std::endl;
            }else if (APP->pModel->Cfg.BrdConfVector[id]->BoardMode == DATA::BrdModes::stop)
            {
                CfgFile << "mode:stop" << std::endl;
            }
            else
            {
                CfgFile << "mode:nonstop" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->State == DATA::BoardState::Off)
            {
                CfgFile << "state:off" << std::endl;
            }
            else
            {
                CfgFile << "state:on" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->write_wheel)
            {
                CfgFile << "write_wheel:on" << std::endl;
            }
            else
            {
                CfgFile << "write_wheel:off" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->write_csv)
            {
                CfgFile << "write_csv:on" << std::endl;
            }
            else
            {
                CfgFile << "write_csv:off" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->write_bin)
            {
                CfgFile << "write_bin:on" << std::endl;
            }
            else
            {
                CfgFile << "write_bin:off" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->write_isa)
            {
                CfgFile << "write_isa:on" << std::endl;
            }
            else
            {
                CfgFile << "write_isa:off" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->write_zero)
            {
                CfgFile << "write_zero:on" << std::endl;
            }
            else
            {
                CfgFile << "write_zero:off" << std::endl;
            }

            CfgFile << "isa_prog_dir:" << APP->pModel->Cfg.BrdConfVector[id]->isa_prog_dir.toStdWString() << std::endl;
            CfgFile << "isa_pause:" << APP->pModel->Cfg.BrdConfVector[id]->isa_pause << std::endl;
            CfgFile << "isa_prg_nomb:" << APP->pModel->Cfg.BrdConfVector[id]->isa_prg_nomb << std::endl;

            if (APP->pModel->Cfg.BrdConfVector[id]->isa_real_time)
            {
                CfgFile << "isa_real_time:on" << std::endl;
            }
            else
            {
                CfgFile << "isa_real_time:off" << std::endl;
            }

            if (APP->pModel->Cfg.BrdConfVector[id]->process)
            {
                CfgFile << "process:on" << std::endl;
            }
            else
            {
                CfgFile << "process:off" << std::endl;
            }

            CfgFile << "datchik_distance:" << APP->pModel->Cfg.BrdConfVector[id]->datchik_distance << std::endl;
            CfgFile << "power:" << APP->pModel->Cfg.BrdConfVector[id]->power << std::endl;

            if (APP->pModel->Cfg.BrdConfVector[id]->side == 0)
            {
                CfgFile << "side:R" << std::endl;
            }
            else
            {
                CfgFile << "side:L" << std::endl;
            }

            CfgFile << "max_timeout:" << APP->pModel->Cfg.BrdConfVector[id]->max_timeout << std::endl;

            CfgFile << "kvant_to_weigth:" << APP->pModel->Cfg.BrdConfVector[id]->kvant_to_weigth << std::endl;

            CfgFile << std::endl;
        }

    }


}

application::~application()
{
    //pModel = new BoardModel(this);
    delete pModel;
}

} // namespace SGDK
