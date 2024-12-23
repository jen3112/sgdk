#include "boardsmodel.h"
#include <map>
#include <vector>
#include <QtSql>
#include <QString>
#include <QDebug>

#include "data.h"
#include "create_dirs.h"
#include "application.h"
#include "wait.h"

namespace SGDK
{

void  BoardModel::fillModelFromCfg()
{
    //читаю общий раздел конфига ( :app )

    if ( Cfg.AppCfg.find(L"archiv_dir") != Cfg.AppCfg.end() )
    {
        Cfg.archiv_dir = QString::fromStdWString(Cfg.AppCfg.at(L"archiv_dir"));

    }
    else Cfg.archiv_dir =  "ARCH\\";

    create_dirs(Cfg.archiv_dir); // если не существуют, то создаю папки для архива


    if ( Cfg.AppCfg.find(L"max_arhive_nomb") != Cfg.AppCfg.end() )
    {
        Cfg.max_arhive_nomb = QString::fromStdWString(Cfg.AppCfg.at(L"max_arhive_nomb")).toInt();
    }

    if ( Cfg.AppCfg.find(L"etalon_com_port_name") != Cfg.AppCfg.end() )
    {
        Cfg.etalon_com_port_name = QString::fromStdWString(Cfg.AppCfg.at(L"etalon_com_port_name")).toInt();
    }
    else Cfg.etalon_com_port_name =  3;


    if ( Cfg.AppCfg.find(L"min_dac_value") != Cfg.AppCfg.end() )
    {
        Cfg.Min_Dac_value = QString::fromStdWString(Cfg.AppCfg.at(L"min_dac_value")).toInt();
    }

    if ( Cfg.AppCfg.find(L"max_dac_value") != Cfg.AppCfg.end() )
    {
        Cfg.Max_Dac_value = QString::fromStdWString(Cfg.AppCfg.at(L"max_dac_value")).toInt();
    }

    if ( Cfg.AppCfg.find(L"recomended_min_dac_value") != Cfg.AppCfg.end() )
    {
        Cfg.Recomended_Min_Dac_value = QString::fromStdWString(Cfg.AppCfg.at(L"recomended_min_dac_value")).toInt();
    }

    if ( Cfg.AppCfg.find(L"recomended_max_dac_value") != Cfg.AppCfg.end() )
    {
        Cfg.Recomended_Max_Dac_value = QString::fromStdWString(Cfg.AppCfg.at(L"recomended_max_dac_value")).toInt();
    }

    if ( Cfg.AppCfg.find(L"update_timer_interval_normal") != Cfg.AppCfg.end() )
    {
        Cfg.update_timer_interval_normal = QString::fromStdWString(Cfg.AppCfg.at(L"update_timer_interval_normal")).toInt();
    }

    if ( Cfg.AppCfg.find(L"update_timer_interval_max") != Cfg.AppCfg.end() )
    {
        Cfg.update_timer_interval_max = QString::fromStdWString(Cfg.AppCfg.at(L"update_timer_interval_max")).toInt();
    }


    Cfg.AppCfg.clear();



    for(auto it = Cfg.BrdCfg.begin(); it!=Cfg.BrdCfg.end(); it++)
    {
        std::map<std::wstring,std::wstring> conf_item_map = it->second;

        // создаю объект конфига на основе map-записи
        DATA::BoardCfg *X = new DATA::BoardCfg(conf_item_map);

        //X->BrdNomb  = it->first;     // № платы

        X->row_nomb_in_table =  it->first;

        Cfg.BrdConfVector.push_back(X);



    }
    Cfg.BrdCfg.clear();
}

void BoardModel::Delete(int row)
{
    beginResetModel();

    auto iter = Cfg.BrdConfVector.cbegin(); // указатель на первый элемент
    delete Cfg.BrdConfVector[row];
    Cfg.BrdConfVector.erase(iter+row);

    endResetModel();

    //сохраняю cfg файл
    APP->save_config();
}

void BoardModel::Edit(DATA::BoardCfg *X)
{
    if (X->subd_id >0)  // если строка из БД а не из конфиг файла
    {
        QString SQL_str = "UPDATE config SET brd_nomb = " + QString::number(X->BrdNomb ) +
                                  ", ip0              = "+QString::number(X->ip[0])+
                                  ", ip1              = "+QString::number(X->ip[1])+
                                  ", ip2              = "+QString::number(X->ip[2])+
                                  ", ip3              = "+QString::number(X->ip[3])+
                                  ", port             = "+QString::number(X->port)+
                                  ", state            = "+ (X->State?QString("'true'"):QString("'false'")) +
                                  ", csv              =  1"
                                  ", bin              =  1"
                                  ", isa              =  1"
                                  ", write_csv        = " + (X->write_csv?QString("'true'"):QString("'false'")) +
                                  ", write_bin        = " + (X->write_bin?QString("'true'"):QString("'false'")) +
                                  ", write_isa        = " + (X->write_isa?QString("'true'"):QString("'false'")) +
                                  ", write_wheel      = " + (X->write_wheel?QString("'true'"):QString("'false'")) +
                                  ", process          = " + (X->process ? QString("'true'"):QString("'false'")) +
                                  ", datchik_distance = " + QString::number(X->datchik_distance)+
                                  ", power            = " + QString::number(X->power)+
                                  ", kvant_to_weigth  = " + QString::number(X->kvant_to_weigth)+
                                  ", side             =  0"
                                  ", isa_prog_dir     = '" + X->isa_prog_dir + "'" +
                                  ", isa_prg_nomb     = " + QString::number(X->isa_prg_nomb)+
                                  ", isa_pause     =    " + QString::number(X->isa_pause)+
                                  ", isa_real_time      = " + (X->isa_real_time?QString("'true'"):QString("'false'")) +

                          " WHERE iid = " + QString::number(X->subd_id );
        QSqlQuery qry;

        qry.prepare(SQL_str);
        qry.setForwardOnly(true);
        qry.exec();
    }

    //сохраняю cfg файл
    APP->save_config();
}

void BoardModel::CreateNew(DATA::BoardCfg *X, unsigned int pozition)
{
    X->ip_str = QString::number(X->ip[0]) + "."
                    + QString::number(X->ip[1]) + "."
                    + QString::number(X->ip[2]) + "."
                    + QString::number(X->ip[3])  ;

    beginResetModel();
    auto iter = Cfg.BrdConfVector.cbegin(); // указатель на первый элемент

    if (pozition <= Cfg.BrdConfVector.size())
    {

        Cfg.BrdConfVector.insert(iter+pozition+1,X);
        //Cfg.BrdConfVector.at(pozition+1)->State = DATA::Off;
    }
    else
    {
        Cfg.BrdConfVector.push_back(X);
    }

    endResetModel();

    //сохраняю cfg файл
    APP->save_config();

}

BoardModel::BoardModel(QObject *parent)
        :QAbstractTableModel(parent)
{
    // Заполнение модели данными из конфига
    BoardModel::fillModelFromCfg();      // Cfg определена статически в классе BoardModel
}


BoardModel::~BoardModel()
{
    // тут надо проверить что z_thread все закрыты

    beginResetModel();
    // перенес это в ~z_thred и ~obrabotka
    /*for (int i = 0; i<Cfg.BrdConfVector.size();i++)
    {
        Cfg.BrdConfVector[i]->State = SGDK::DATA::Off;
        Cfg.BrdConfVector[i]->z_thr.train_obrab.stop = true;
        //Cfg.BrdConfVector[0]->gr_win.close();
    }*/

    while (!Cfg.BrdConfVector.empty())
    {
        auto iter = Cfg.BrdConfVector.cbegin(); // указатель на первый элемент

        // перенес это в ~z_thred и ~obrabotka
        //wait_until_false(&Cfg.BrdConfVector[0]->z_thr.z_run);
        //wait_until_true(&Cfg.BrdConfVector[0]->z_thr.train_obrab.ready_to_delete);

        delete Cfg.BrdConfVector[0];
        Cfg.BrdConfVector.erase(iter);
    }
    endResetModel();

    //Cfg.BrdConfVector.clear();

    // Объект Cfg создан автоматически > удалять не надо
}

int BoardModel::rowCount(const QModelIndex & parent ) const
{
    if (parent.isValid()) return 0;

    return Cfg.BrdConfVector.size();
}

int BoardModel::columnCount(const QModelIndex & parent ) const
{
    if (parent.isValid()) return 0;
    return 33;
}

QVariant BoardModel::data( const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column()) {

        case 0:  // № платы
            return QString::number(Cfg.BrdConfVector.at(index.row())->BrdNomb) + "-" +
                    ((Cfg.BrdConfVector.at(index.row())->side == 0)?"R":"L");
        case 1:  // IP адрес
            return QString::number(Cfg.BrdConfVector.at(index.row())->ip[0]) + "." +
                   QString::number(Cfg.BrdConfVector.at(index.row())->ip[1]) + "." +
                   QString::number(Cfg.BrdConfVector.at(index.row())->ip[2]) + "." +
                   QString::number(Cfg.BrdConfVector.at(index.row())->ip[3]) ;
        case 2:  // порт
            return QString::number( Cfg.BrdConfVector.at(index.row())->port );
        case 3:  // Статус
            switch (Cfg.BrdConfVector.at(index.row())->State)
            {
                case DATA::BoardState::On:
                {
                    QString S="On_";
                    return S;
                    break;
                }
                case DATA::BoardState::Off:
                {
                    QString S="Off";
                    return S;
                    break;
                }
                case DATA::BoardState::Connected:
                {
                    QString S="Con";
                    return S;
                    break;
                }
                case DATA::BoardState::Connecting:
                {
                    QString S="Try";
                    return S;
                    break;
                }
                case DATA::BoardState::Train_detection:
                {
                    QString S="TRN";
                    return S;
                    break;
                }
                case DATA::BoardState::Disconnected:
                {
                    QString S="Dis";
                    return S;
                    break;
                }
                default:
                {
                    QString S="Off";
                    return S;
                    break;
                }
            }
        case 4: return Cfg.BrdConfVector.at(index.row())->DAC[0];  //DAC0
        case 5: return Cfg.BrdConfVector.at(index.row())->DAC[1];  //DAC1
        case 6: return Cfg.BrdConfVector.at(index.row())->DAC[2];  //DAC2
        case 7: return Cfg.BrdConfVector.at(index.row())->DAC[3];  //DAC3

        case 8: return Cfg.BrdConfVector.at(index.row())->ADC[0];  //ADC0
        case 9: return Cfg.BrdConfVector.at(index.row())->ADC[1];  //ADC1
        case 10:return Cfg.BrdConfVector.at(index.row())->ADC[2];  //ADC2
        case 11:return Cfg.BrdConfVector.at(index.row())->ADC[3];  //ADC3




        case 12: if (Cfg.temperature_mode == SGDK::DATA::kvant)
                        return Cfg.BrdConfVector.at(index.row())->Temp_kvanty[0];  //температура рельса
                 else
                    return (float)Cfg.BrdConfVector.at(index.row())->Temp_grad[0]/10;
        case 13: if (Cfg.temperature_mode == SGDK::DATA::kvant)
                        return Cfg.BrdConfVector.at(index.row())->Temp_kvanty[1];  //температура рельса
                 else   return (float)Cfg.BrdConfVector.at(index.row())->Temp_grad[1]/10;
        case 14: if (Cfg.temperature_mode == SGDK::DATA::kvant)
                        return Cfg.BrdConfVector.at(index.row())->Temp_kvanty[2];  //температура рельса
                 else   return (float)Cfg.BrdConfVector.at(index.row())->Temp_grad[2]/10;
        case 15: if (Cfg.temperature_mode == SGDK::DATA::kvant)
                        return Cfg.BrdConfVector.at(index.row())->Temp_kvanty[3];  //температура рельса
                 else   return (float)Cfg.BrdConfVector.at(index.row())->Temp_grad[3]/10;




        case 16:return Cfg.BrdConfVector.at(index.row())->FREQ;      //частота

        case 17:return Cfg.BrdConfVector.at(index.row())->BrdVersion;

        case 18:return Cfg.BrdConfVector.at(index.row())->BrdTime;

        case 19:return Cfg.BrdConfVector.at(index.row())->BrdDacType;





        case 20:return Cfg.BrdConfVector.at(index.row())->sko[0];
        case 21:return Cfg.BrdConfVector.at(index.row())->sko[1];
        case 22:return Cfg.BrdConfVector.at(index.row())->sko[2];
        case 23:return Cfg.BrdConfVector.at(index.row())->sko[3];

        case 24:return Cfg.BrdConfVector.at(index.row())->delta[0];
        case 25:return Cfg.BrdConfVector.at(index.row())->delta[1];
        case 26:return Cfg.BrdConfVector.at(index.row())->delta[2];
        case 27:return Cfg.BrdConfVector.at(index.row())->delta[3];
        case 28:return Cfg.BrdConfVector.at(index.row())->PowerManagementType;
        case 29:return Cfg.BrdConfVector.at(index.row())->ch_nomb;
        case 30:return Cfg.BrdConfVector.at(index.row())->temperature_limit;
        case 31:return Cfg.BrdConfVector.at(index.row())->power_mode;

        //BOARD.z_thread_status
        case 32:return Cfg.BrdConfVector.at(index.row())->z_thread_status;

        default:return QVariant();
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont F;
        //int dat_iid = Cfg.BrdConfVector.at(I.row()).iid;
        if (Cfg.BrdConfVector.at(index.row())->subd_id  >= 0 )
        {
            F.setBold(true);
            return F;
        }

        else return QVariant();
    }
    else if (role == Qt::ForegroundRole)
    {
        //int row = index.row();
        //int col = index.column();

        // если датчики отключены из-за перегрева, то надпись красным
        if (Cfg.BrdConfVector.at(index.row())->power_mode == 0 &&
                (Cfg.BrdConfVector.at(index.row())->State == DATA::Connected || Cfg.BrdConfVector.at(index.row())->State == DATA::Train_detection))
        {
            return QColor(Qt::red);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        int row = index.row();
        int col = index.column();
        QColor color = calculateColorForRow(row,col);

        // определение не вылез ли цап за диапазон
        if  ((col>3) && (col<8))
        {
            int DAC_column = col-4;
            QColor Dark = QColor::fromRgb(255,100,100);
            QColor Light = QColor::fromRgb(255,240,240);

            if (Cfg.BrdConfVector.at(index.row())->DAC[DAC_column]>Cfg.Recomended_Max_Dac_value)
            {
                color = Light;
            }
            if (Cfg.BrdConfVector.at(index.row())->DAC[DAC_column]>Cfg.Max_Dac_value)
            {
                color = Dark;
            }
            if ((Cfg.BrdConfVector.at(index.row())->DAC[DAC_column] < Cfg.Recomended_Min_Dac_value) && (Cfg.BrdConfVector.at(index.row())->DAC[DAC_column] > 0))
            {
                color = Light;
            }
            if ((Cfg.BrdConfVector.at(index.row())->DAC[DAC_column] < Cfg.Min_Dac_value) && (Cfg.BrdConfVector.at(index.row())->DAC[DAC_column] > 0))
            {
                color = Dark;
            }
        }
        return QBrush(color);
    }

    return QVariant();

}

QColor BoardModel::calculateColorForRow(int row, int col) const
{
    if (Cfg.BrdConfVector.at(row)->write_custom_bin && Cfg.BrdConfVector.at(row)->write_custom_csv && (col == 1 || col ==0))
    { // включена запись csv и bin одновременно
        return QColor::fromRgb(255,255,0);
    }

    if (Cfg.BrdConfVector.at(row)->write_custom_bin && (col == 1 || col ==0))
    {
        return QColor::fromRgb(0,255,255);
    }
    if (Cfg.BrdConfVector.at(row)->write_custom_csv && (col == 1 || col ==0))
    {
        return QColor::fromRgb(255,0,255);
    }

    if (Cfg.BrdConfVector.at(row)->BoardMode == DATA::BrdModes::test)
    {
       return QColor::fromRgb(230,255,230);
    }
    if (Cfg.BrdConfVector.at(row)->BoardMode == DATA::BrdModes::nonstop)
    {
       return QColor::fromRgb(255,255,230);
    }

    return QColor::fromRgb(255,255,255);
}

QVariant BoardModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)    return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:   return QVariant(QString::fromUtf8("№"));
            case 1:   return QVariant(QString::fromUtf8("IP Addr"));
            case 2:   return QVariant(QString::fromUtf8("Prt"));
            case 3:   return QVariant(QString::fromUtf8("State"));
            case 4:   return QVariant(QString::fromUtf8("ЦАП1"));
            case 5:   return QVariant(QString::fromUtf8("ЦАП2"));
            case 6:   return QVariant(QString::fromUtf8("ЦАП3"));
            case 7:   return QVariant(QString::fromUtf8("ЦАП4"));
            case 8:   return QVariant(QString::fromUtf8("АЦП1"));
            case 9:   return QVariant(QString::fromUtf8("АЦП2"));
            case 10:   return QVariant(QString::fromUtf8("АЦП3"));
            case 11:   return QVariant(QString::fromUtf8("АЦП4"));

            case 12:   if (Cfg.temperature_mode == SGDK::DATA::kvant)
                              return QVariant(QString::fromUtf8("T1(Кв)"));  //температура рельса
                       else   return QVariant(QString::fromUtf8("T1(°C)"));


            case 13:   if (Cfg.temperature_mode == SGDK::DATA::kvant)
                                return QVariant(QString::fromUtf8("T2(Кв)"));  //температура рельса
                         else   return QVariant(QString::fromUtf8("T2(°C)"));

            case 14:   if (Cfg.temperature_mode == SGDK::DATA::kvant)
                                return QVariant(QString::fromUtf8("T3(Кв)"));  //температура рельса
                         else   return QVariant(QString::fromUtf8("T3(°C)"));

            case 15:   if (Cfg.temperature_mode == SGDK::DATA::kvant)
                                return QVariant(QString::fromUtf8("T4(Кв)"));  //температура рельса
                         else   return QVariant(QString::fromUtf8("T4(°C)"));

            case 16:   return QVariant(QString::fromUtf8("F"));
            case 17:   return QVariant(QString::fromUtf8("Ver"));
            case 18:   return QVariant(QString::fromUtf8("Time"));
            case 19:   return QVariant(QString::fromUtf8("D-type"));

            case 20:   return QVariant(QString::fromUtf8("СКО 1"));
            case 21:   return QVariant(QString::fromUtf8("СКО 2"));
            case 22:   return QVariant(QString::fromUtf8("СКО 3"));
            case 23:   return QVariant(QString::fromUtf8("СКО 4"));

            case 24:   return QVariant(QString::fromUtf8("d 1"));
            case 25:   return QVariant(QString::fromUtf8("d 2"));
            case 26:   return QVariant(QString::fromUtf8("d 3"));
            case 27:   return QVariant(QString::fromUtf8("d 4"));

            case 28:   return QVariant(QString::fromUtf8("P-type"));
            case 29:   return QVariant(QString::fromUtf8("Ch"));

            case 30:   return QVariant(QString::fromUtf8("T-limit"));
            case 31:   return QVariant(QString::fromUtf8("P-mode"));

            case 32:   return QVariant(QString::fromUtf8("Z-state"));

            default:  return QVariant();
        }
    }
    return QVariant();
}

} // namespace SGDK
