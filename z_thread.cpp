#include <WinSock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "time_str.h"
#include "z_thread.h"
//#include "data.h"
#include "application.h"
#include "ring_buf.h"
#include "int_ring_buf.h"
#include "wait.h"



namespace SGDK
{

    void Z_thread::Z_thr_init(DATA::BoardCfg *brd_cfg_pointer)
    {
        // заполняю свойства объекта
        z_brd_cfg_pointer = brd_cfg_pointer;
        train_obrab.board_obrab_cfg_pointer = brd_cfg_pointer;
    }



    Z_thread::Z_thread(DATA::BoardCfg *brd_cfg_pointer)
    {
        // заполняю свойства объекта
        z_brd_cfg_pointer = brd_cfg_pointer;
        train_obrab.board_obrab_cfg_pointer = brd_cfg_pointer;
    }

    Z_thread::Z_thread() {}

    Z_thread::~Z_thread()
    {
        z_brd_cfg_pointer->State = SGDK::DATA::Off;
        wait_until_false(&z_run);
    }

/*------------------------------------------------------------------------------*/

    void Z_thread::GetTempChanelData()
    {
        // определяю № платы
        z_brd_cfg_pointer->BrdNomb  =  z_brd_cfg_pointer->buff[1];
        unsigned short T  = (z_brd_cfg_pointer->buff[4] << 8) + (z_brd_cfg_pointer->buff[5]);
        switch (z_brd_cfg_pointer->buff[2])   // Чтение данных из термоканала
        {
        case 0:  // определяю температуру 1 канал
            z_brd_cfg_pointer->Temp_kvanty[0] = T;
            break;
        case 1:  // определяю температуру 1 канал
            z_brd_cfg_pointer->Temp_kvanty[0] = T;
            break;
        case 2:  // определяю температуру 2 канал
            z_brd_cfg_pointer->Temp_kvanty[1] = T;
            break;
        case 3:  // определяю температуру 2 канал
            z_brd_cfg_pointer->Temp_kvanty[1] = T;
            break;
        case 4:  // значение DAC0
            z_brd_cfg_pointer->DAC[0] = T;
            break;
        case 5:  // значение DAC0
            z_brd_cfg_pointer->DAC[0] = T;
            break;
        case 6:  // значение DAC1
            z_brd_cfg_pointer->DAC[1] = T;
            break;
        case 7:  // значение DAC1
            z_brd_cfg_pointer->DAC[1] = T;
            break;
        case 8:  // значение DAC2
            z_brd_cfg_pointer->DAC[2] = T;
            break;
        case 9:  // значение DAC2
            z_brd_cfg_pointer->DAC[2] = T;
            break;
        case 10:  // значение DAC3
            z_brd_cfg_pointer->DAC[3] = T;
            break;
        case 11:  // значение DAC3
            z_brd_cfg_pointer->DAC[3] = T;
            break;
        case 12:  // значение частоты
            z_brd_cfg_pointer->FREQ = z_brd_cfg_pointer->buff[5];
            break;
        case 13:  // значение частоты
            z_brd_cfg_pointer->FREQ = z_brd_cfg_pointer->buff[5];
            break;
        case 20:  // значение версии прошивки
            z_brd_cfg_pointer->BrdVersion = QString::number(z_brd_cfg_pointer->buff[4]) + "." + QString::number(z_brd_cfg_pointer->buff[5]);
            break;
        case 21:  // значение версии прошивки
            //QString Str =  buff[4]+"."+buff[5];
            z_brd_cfg_pointer->BrdVersion = QString::number(z_brd_cfg_pointer->buff[4]) + "." + QString::number(z_brd_cfg_pointer->buff[5]);
            break;
        case 22:  // значение времени работы платы // старшие разряды
            z_brd_cfg_pointer->BrdTime_high =  ((z_brd_cfg_pointer->buff[4] << 8) + z_brd_cfg_pointer->buff[5]) << 16;
            break;
        case 23:  // значение времени работы платы // старшие разряды
            z_brd_cfg_pointer->BrdTime_high =  ((z_brd_cfg_pointer->buff[4] << 8) + z_brd_cfg_pointer->buff[5]) << 16;
            break;
        case 24:  // значение времени работы платы // младшие разряды
            z_brd_cfg_pointer->BrdTime = z_brd_cfg_pointer->BrdTime_high + ((z_brd_cfg_pointer->buff[4] << 8) + z_brd_cfg_pointer->buff[5]);
            break;
        case 25:  // значение времени работы платы // младшие разряды
            z_brd_cfg_pointer->BrdTime = z_brd_cfg_pointer->BrdTime_high + ((z_brd_cfg_pointer->buff[4] << 8) + z_brd_cfg_pointer->buff[5]);
            break;
        case 26:  // BrdDacType
            z_brd_cfg_pointer->BrdDacType =  T;
            break;
        case 27:  // BrdDacType
            z_brd_cfg_pointer->BrdDacType =  T;
            break;
        case 28:  // тип управления питанием
            z_brd_cfg_pointer->PowerManagementType =  T;
            break;
        case 29:  // тип управления питанием
            z_brd_cfg_pointer->PowerManagementType =  T;
            break;
        case 50:  // определяю температуру 1 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[0] = T;}
            }
            break;
        case 51:  // определяю температуру 1 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[0] = T;}
            }
            break;
        case 52:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[1] = T;}
            }
            break;
        case 53:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[1] = T;}
            }
            break;
        case 54:  // определяю температуру 3 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[2] = T;}
            }
            break;
        case 55:  // определяю температуру 3 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[2] = T;}
            }
            break;
        case 56:  // определяю температуру 4 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[3] = T;}
            }
            break;
        case 57:  // определяю температуру 4 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_kvanty[3] = T;}
            }
            break;

        // для 16-канальной платы
        case 58:
            if (z_brd_cfg_pointer->ch_nomb != T)
            {
                z_brd_cfg_pointer->ch_nomb_changed = 1;
                z_brd_cfg_pointer->ch_nomb = T;
            }
            break;
        case 59:
            if (z_brd_cfg_pointer->ch_nomb != T)
            {
                z_brd_cfg_pointer->ch_nomb_changed = 1;
                z_brd_cfg_pointer->ch_nomb = T;
            }
            break;

        case 60:  // значение DAC4
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[0] = T;
            }
            break;
        case 61:  // значение DAC4
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[0] = T;
            }
            break;
        case 62:  // значение DAC5
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[1] = T;
            }
            break;
        case 63:  // значение DAC5
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[1] = T;
            }
            break;
        case 64:  // значение DAC6
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[2] = T;
            }
            break;
        case 65:  // значение DAC6
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[2] = T;
            }
            break;
        case 66:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[3] = T;
            }
            break;
        case 67:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[3] = T;
            }
            break;
        case 68:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[4] = T;
            }
            break;
        case 69:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[4] = T;
            }
            break;
        case 70:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[5] = T;
            }
            break;
        case 71:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[5] = T;
            }
            break;
        case 72:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[6] = T;
            }
            break;
        case 73:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[6] = T;
            }
            break;
        case 74:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[7] = T;
            }
            break;
        case 75:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[7] = T;
            }
            break;
        case 76:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[8] = T;
            }
            break;
        case 77:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[8] = T;
            }
            break;
        case 78:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[9] = T;
            }
            break;
        case 79:  // значение DAC7
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[9] = T;
            }
            break;
        case 80:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[10] = T;
            }
            break;
        case 81:  // значение DAC8
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[10] = T;
            }
            break;
        case 82:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[11] = T;
            }
            break;
        case 83:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[11] = T;
            }
            break;
        case 84:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[12] = T;
            }
            break;
        case 85:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[12] = T;
            }
            break;
        case 86:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[13] = T;
            }
            break;
        case 87:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[13] = T;
            }
            break;
        case 88:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[14] = T;
            }
            break;
        case 89:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[14] = T;
            }
            break;
        case 90:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[15] = T;
            }
            break;
        case 91:  // значение DAC9
            if (z_brd_cfg_pointer->ch_nomb == 16)
            {
                z_brd_cfg_pointer->DAC[15] = T;
            }
            break;

        case 92:
            z_brd_cfg_pointer->power_mode = T;
            break;
        case 93:
            z_brd_cfg_pointer->power_mode = T;
            break;
        case 94:
            z_brd_cfg_pointer->temperature_limit = T;
            break;
        case 95:
            z_brd_cfg_pointer->temperature_limit = T;
            break;


        case 150:  // определяю температуру 1 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[0] = T;}
            }
            break;
        case 151:  // определяю температуру 1 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[0] = T;}
            }
            break;
        case 152:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[1] = T;}
            }
            break;
        case 153:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[1] = T;}
            }
            break;
        case 154:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[2] = T;}
            }
            break;
        case 155:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[2] = T;}
            }
            break;
        case 156:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[3] = T;}
            }
            break;
        case 157:  // определяю температуру 2 канал
            {
                if (T > 0) {z_brd_cfg_pointer->Temp_grad[3] = T;}
            }
            break;

        }
    }

/*----------- подсчет и вывод на экран СКО и ДЕЛЬТА ------------------*/

    void Z_thread::count_sko()
    {
        for (int i=0; i<16; i++)
        {
            //int tim =( (buff[3]<<8) + buff[2] ) * 16 + i; // это время пакета, надо его еще умножить на 16 точек в пакете
            for (int j=0;j<4;j++)
            {
                z_brd_cfg_pointer->sko_buff[j][z_brd_cfg_pointer->sko_buff_poz] = (z_brd_cfg_pointer->buff[7+2*j+8*i]<<8) + z_brd_cfg_pointer->buff[6+2*j+8*i];
            }

            z_brd_cfg_pointer->sko_buff_poz ++;

            if ( z_brd_cfg_pointer->sko_buff_poz >= SCO_BUFF_LEN )
            {
                // подсчет СКО, MIN и MAX
                int min[4] {100000,100000,100000,100000};
                int max[4] {0,0,0,0};
                int SKO_local[4] {0,0,0,0};
                int SKO[4] {0,0,0,0};

                // считаю среднее в 4 точках
                // и отклонение в каждой области от своего среднего
                for (int j = 0; j<4; j++)
                {
                    long int summ[4] {0,0,0,0};
                    long int middle[4];

                    for (int k=0; k<SCO_BUFF_LEN/4; k++)
                    {
                        for (int z = 0; z<4; z++)
                        {
                            summ[z] += z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k];

                            if ( min[z] > z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k] )
                            {
                                min[z] = z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k] ;
                            }
                            if ( max[z] < z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k] )
                            {
                                max[z] = z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k] ;
                            }
                        }
                     }
                     for(int counter=0; counter<4; counter++)
                     {
                         middle[counter] = (summ[counter]/SCO_BUFF_LEN)*4;
                     }

                     for (int k=0; k<SCO_BUFF_LEN/4; k++)
                     {
                         // подсчет локального СКО в миниобласти
                         for (int z=0; z<4; z++)
                         {
                             int diff = middle[z] - z_brd_cfg_pointer->sko_buff[z][j*SCO_BUFF_LEN/4 + k];

                             if (diff<0) diff=-diff;

                             SKO_local[z] += diff;
                         }
                     }
                     for (int z=0; z<4; z++)
                     {
                         SKO[z] += (SKO_local[z] * 4) / SCO_BUFF_LEN;
                     }
                }
                for (int z=0; z<4; z++)
                {
                     // вывод СКО на экран
                     z_brd_cfg_pointer->sko[z] = SKO[z]/4;
                     z_brd_cfg_pointer->delta[z] = max[z] - min[z];
                }
                z_brd_cfg_pointer->sko_buff_poz = 0;
                i = 17;  // для принудительного выхода из цикла если подсчет закончен
            }
        }
    };

/*------------------------------------------------------------------------------*/

    void Z_thread::fill_graph_buff()
    {
        if(z_brd_cfg_pointer->gr_win.graph_is_started &&
                (z_brd_cfg_pointer->gr_win.gr_time.size() >= z_brd_cfg_pointer->gr_win.gr_buff_len-1))
        {
            // Если не изменено кол-во каналов (иначе вылетает при горячем подключении платы)
            if (!z_brd_cfg_pointer->ch_nomb_changed)
            {
                unsigned int val[17];
                int ch_n = z_brd_cfg_pointer->ch_nomb;
                if (ch_n != 4 && ch_n != 16) ch_n = 4;

                z_brd_cfg_pointer->gr_win.gr_check_box_change();  // Если убрать то при старте графиков не все отображаются

                int points_per_packet = 64 / ch_n;
                int point_len = 128/points_per_packet;

                for (int i=0; i<points_per_packet; i++)
                {
                    // определяю время и значения квантов
                    val[0] =( (z_brd_cfg_pointer->buff[3]<<8) + z_brd_cfg_pointer->buff[2] ) * points_per_packet + i; // это время пакета, надо его еще умножить на 16 точек в пакете

                    for (int j=1; j<ch_n+1; j++)
                    {
                        val[j] = (z_brd_cfg_pointer->buff[5+2*j+point_len*i]<<8) + z_brd_cfg_pointer->buff[4+2*j+point_len*i];
                    }

                    // пишу в graph.CSV файл
                    write_csv(&z_brd_cfg_pointer->gr_win.graph_file, val, ch_n+1);

                    //APP->gr_wins[z_brd_cfg_pointer]->gr_time[APP->gr_wins[z_brd_cfg_pointer]->buff_pozition] = val[0];
                    z_brd_cfg_pointer->gr_win.gr_time[z_brd_cfg_pointer->gr_win.buff_pozition] = val[0];

                    int summ = 0;
                    for (int j=0; j<ch_n; j++)
                    {
                        summ += z_brd_cfg_pointer->gr_win.gr_y[j][z_brd_cfg_pointer->gr_win.buff_pozition] = val[j+1];
                        //summ += val[j+1];
                    }

                    z_brd_cfg_pointer->gr_win.gr_y[16][z_brd_cfg_pointer->gr_win.buff_pozition] = summ/ch_n;

                    z_brd_cfg_pointer->gr_win.buff_pozition ++;

                    if (z_brd_cfg_pointer->gr_win.buff_pozition  >  z_brd_cfg_pointer->gr_win.gr_buff_len)
                    {
                        z_brd_cfg_pointer->gr_win.offset(1000); // сдвиг графика на 1000 точек
                        z_brd_cfg_pointer->gr_win.buff_pozition -= 1000;
                    }
                }
            }
            else   // (!BOARD.ch_nomb_changed)
            {
                // для очистки буфера
                z_brd_cfg_pointer->gr_win.gr_time.fill(0,z_brd_cfg_pointer->gr_win.gr_buff_len+1);

                for (int n=0; n<17; n++)
                {
                    z_brd_cfg_pointer->gr_win.gr_y[n].fill(0,z_brd_cfg_pointer->gr_win.gr_buff_len+1);
                }
            }

        }
    }

/*------------------------------------------------------------------------------*/

    void Z_thread::fill_membrane_graph_buff()
    {
/*        if(z_brd_cfg_pointer->start_membrane_graph == 1)
        {
            unsigned int val_x[3];

            // определяю время
            val_x[0] =( (z_brd_cfg_pointer->buff[3]<<8) + z_brd_cfg_pointer->buff[2] ) ; // это время пакета, надо его еще умножить на 16 точек в пакете

            // определяю среднее значение квантов
            for (int i=0; i<16; i++)
            {

                for (int j=1; j<3; j++)
                {
                    val_x[j] = (z_brd_cfg_pointer->buff[5+2*j+8*i]<<8) + z_brd_cfg_pointer->buff[4+2*j+8*i];
                }

                // пишу в graph.CSV файл
                write_csv(&APP->membrane_wins[z_brd_cfg_pointer]->graph_file, val_x, 3);

                APP->membrane_wins[z_brd_cfg_pointer]->gr_x[APP->membrane_wins[z_brd_cfg_pointer]->buff_pozition] = val_x[1];
                APP->membrane_wins[z_brd_cfg_pointer]->gr_y[APP->membrane_wins[z_brd_cfg_pointer]->buff_pozition] = val_x[2];

                APP->membrane_wins[z_brd_cfg_pointer]->buff_pozition ++;

                if (APP->membrane_wins[z_brd_cfg_pointer]->buff_pozition > APP->membrane_wins[z_brd_cfg_pointer]->gr_buff_len)
                {
                    APP->membrane_wins[z_brd_cfg_pointer]->offset(1000); // сдвиг графика на 1000 точек
                    APP->membrane_wins[z_brd_cfg_pointer]->buff_pozition -= 1000;
                }
            }
        }*/
    }

/*------------------------------------------------------------------------------*/

    void Z_thread::z_thread_init()
    {
        //ready_to_delete = false;
        // Инициализация дочернего объекта isa_rt_thread
        isa_rt_thread.board_id = nomber_in_cfg_list;

        // Инициализация временных бинарников
        temp_bin_file_name1 = "out_"
                + QString::number(nomber_in_cfg_list)
                + "_1.bin";
        temp_bin_file_name2 = "out_"
                + QString::number(nomber_in_cfg_list)
                + "_2.bin";

        z_brd_cfg_pointer->bin_file_name = "out_"
                + QString::number(nomber_in_cfg_list)
                + ".bin";

        z_brd_cfg_pointer->isa_file_name = "out_"
                + QString::number(nomber_in_cfg_list)
                + ".isa";

        z_brd_cfg_pointer->csv_file_name = "out_"
                + QString::number(nomber_in_cfg_list)
                + ".csv";

        // Инициализация файла записи нулей
        zero_file_name = APP->pModel->Cfg.archiv_dir +
                               "zero_" + QString::number(z_brd_cfg_pointer->BrdNomb) + ".csv" ;

        // адрес сервера. Т.к. TCP/IP представляет адреса в числовом виде, то для перевода
        // адреса используем функцию inet_pton. сделает из 192.168.0.024 16тиричное понятное сокету предстравление
        //inet_pton(AF_INET, "192.168.0.7", &saddr.sin_addr.s_addr);
        // Порт. Используем функцию htons для перевода номера порта из обычного в //TCP/IP представление.

        // Заполним данными кроме адреса и порта
        ZeroMemory(&saddr, sizeof(saddr));

        // тип адреса (TCP/IP)
        saddr.sin_family = AF_INET; //стандартная инициализация
        saddr.sin_port = htons( z_brd_cfg_pointer->port );
        //преобразование QString Ip_str в char*
        saddr.sin_addr.s_addr=inet_addr(z_brd_cfg_pointer->ip_str.toStdString().c_str());

        // меняю статус платы
        //BOARD.State = SGDK::DATA::Connecting;
    }

    /*------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------*/

    bool Z_thread::SetSocketBlockingEnabled(int fd, bool blocking)
    {
       if (fd < 0) return false;

    #ifdef _WIN32
       unsigned long mode = blocking ? 0 : 1;
       return (ioctlsocket(fd, FIONBIO, &mode) == 0);
    #else
       int flags = fcntl(fd, F_GETFL, 0);
       if (flags == -1) return false;
       flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
       return (fcntl(fd, F_SETFL, flags) == 0);
    #endif
    }

    /*------------------------------------------------------------------------------*/

    int Z_thread::socket_init()
    {
        //...  инициализация Winsock
        if (FAILED(::WSAStartup(MAKEWORD(1, 1), &ws)))
        {
            // Error...
            z_brd_cfg_pointer->z_thread_status=9;
            error = ::WSAGetLastError();
            std::string S = "Thread " + std::to_string(z_brd_cfg_pointer->BrdNomb) + " WSA init Error";
            //SGDK::DATA::config::write_log(S);
            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }
            //ready_to_delete = true;
            z_brd_cfg_pointer->z_thread_status=10;
            return error;
        }
        /*z_brd_cfg_pointer->z_thread_status=11;
        // создание сокета
        if (INVALID_SOCKET == (z_brd_cfg_pointer->s = ::socket(AF_INET, SOCK_STREAM, 0)))     // Канал А
        {
            // Error...
            error = WSAGetLastError();
            // ...
            std::string Str = "Thread " + std::to_string(BOARD->BrdNomb) + " SOCKET create Error";
            //SGDK::DATA::config::write_log(Str);

            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }
            ready_to_delete = true;
            z_brd_cfg_pointer->z_thread_status=12;
            return error;
        };
        */
       z_brd_cfg_pointer->z_thread_status=13;
        // создание сокета
        if (INVALID_SOCKET == (z_brd_cfg_pointer->s = ::socket(AF_INET, SOCK_STREAM, 0)))     // Канал А
        {
            // Error...
            error = WSAGetLastError();
            // ...
            std::string Str = "Thread " + std::to_string(z_brd_cfg_pointer->BrdNomb) + " SOCKET create Error";
            //SGDK::DATA::config::write_log(Str);

            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }
            //ready_to_delete = true;
            z_brd_cfg_pointer->z_thread_status=14;
            return error;
        };
        z_brd_cfg_pointer->z_thread_status=15;
        /*int ret=*/ setsockopt(z_brd_cfg_pointer->s,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));  //установка таймаута сокета

        // --- КОНЕЦ --- создания сокета (вариант API) -------------

        z_brd_cfg_pointer->z_thread_status=16;


        // перевожу в неблокирующий режим
        SetSocketBlockingEnabled(z_brd_cfg_pointer->s, false);

        // выполняем соединение:
        long int res = ::connect(z_brd_cfg_pointer->s, (sockaddr*)&saddr, sizeof(saddr));
        if (res == -1)
        {
            if (WSAEWOULDBLOCK != WSAGetLastError())
            {
                // connect failed, do something...
                error = WSAGetLastError();
                return error;
            }

            fd_set wfd, efd;

            FD_ZERO(&wfd);
            FD_SET(z_brd_cfg_pointer->s, &wfd);

            FD_ZERO(&efd);
            FD_SET(z_brd_cfg_pointer->s, &efd);

            // Timeout for socket connection
            timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 800;

            res = select(z_brd_cfg_pointer->s+1, NULL, &wfd, &efd, &tv);
            if (res == -1)
            {
                // select failed, do something...
                error = WSAGetLastError();
                return error;
            }

            if (res == 0)
            {
                // connect timed out...
                error = WSAGetLastError();
                return error;
            }

            if (FD_ISSET(z_brd_cfg_pointer->s, &efd))
            {
                // connect failed, do something...
                //getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
                error = WSAGetLastError();
                return error;
            }
        }

        // перевожу в блокирующий режим
        SetSocketBlockingEnabled(z_brd_cfg_pointer->s, true);








        // выполняем соединение:
        /*if (SOCKET_ERROR == (::connect(z_brd_cfg_pointer->s, (sockaddr*)&saddr, sizeof(saddr))))
        {
            // Error...
            error = WSAGetLastError();
            std::string S = "Thread " + std::to_string(z_brd_cfg_pointer->BrdNomb) + " connection Error";
            //SGDK::DATA::config::write_log(S);

            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }
            ready_to_delete = true;
            z_brd_cfg_pointer->z_thread_status=17;
            return error;
        } */
        z_brd_cfg_pointer->z_thread_status=18;
        // Считываю конец последнего пакета тк обрывок наверняка там сидит (кусок старого пакета)
        if (SOCKET_ERROR == (actual_len = recv(z_brd_cfg_pointer->s, (char*)&z_brd_cfg_pointer->buff, len, 0)))//из сокета s считали в buff  len байт
        {
            // Error... //cout << "Error reading SOCKET";
            error = WSAGetLastError();

            std::string S = "Thread "+std::to_string(z_brd_cfg_pointer->BrdNomb)+" socket read Error";
            //SGDK::DATA::config::write_log(S);

            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }
            //ready_to_delete = true;

            z_brd_cfg_pointer->z_thread_status=19;

            return error;
        }
        z_brd_cfg_pointer->z_thread_status=20;
        return 0;
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::clear_train_data()
    {
        if (!APP->wheels_all.empty())     APP->wheels_all.clear();
        if (!APP->wheels_average.empty()) APP->wheels_average.clear();
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::fill_zero_buff()
    {
        if (z_brd_cfg_pointer->write_zero)
        {
            //int  write_zero_interval = 300 по умолчанию; // в секундах (*125 = в пакетах)
            if (zero_interval_counter++ > z_brd_cfg_pointer->write_zero_interval*125)
            {
                zero_interval_counter = 0;
                zeroes_buff[zero_buff_pos][0] = (z_brd_cfg_pointer->buff[7]<<8) + z_brd_cfg_pointer->buff[6];
                zeroes_buff[zero_buff_pos][1] = (z_brd_cfg_pointer->buff[9]<<8) + z_brd_cfg_pointer->buff[8];
                zeroes_buff[zero_buff_pos][2] = (z_brd_cfg_pointer->buff[11]<<8) + z_brd_cfg_pointer->buff[10];
                zeroes_buff[zero_buff_pos][3] = (z_brd_cfg_pointer->buff[13]<<8) + z_brd_cfg_pointer->buff[12];
                // 4-7 - температура
                zeroes_buff[zero_buff_pos][4] = z_brd_cfg_pointer->Temp_kvanty[0];
                zeroes_buff[zero_buff_pos][5] = z_brd_cfg_pointer->Temp_kvanty[1];
                zeroes_buff[zero_buff_pos][6] = z_brd_cfg_pointer->Temp_kvanty[2];
                zeroes_buff[zero_buff_pos][7] = z_brd_cfg_pointer->Temp_kvanty[3];

                zero_buff_pos++;
                if (zero_buff_pos == 10)
                {
                    write_zero_buff_to_file();
                    zero_buff_pos=0;
                }
            }

        }
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::write_zero_buff_to_file()
    {
        std::ofstream zero_file ;
        zero_file.open( zero_file_name.toStdString() , std::ios::app);
        zero_file << get_time_str().toStdString() << ";"  << std::endl;
        for(int i=0;i<10;i++)
        {
            for (int k=0; k<8; k++)
            {
                zero_file << QString::number(zeroes_buff[i][k]).toStdString() <<  ";";
            }
            zero_file << std::endl;
        }
        zero_file.close();
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::run()  // переопределение метода который будет запущен в новом потоке
    {
        z_brd_cfg_pointer->z_thr.z_run = true;
        z_brd_cfg_pointer->z_thread_status = 1;
        z_thread_init();
        z_brd_cfg_pointer->z_thread_status = 2;
        error = socket_init();
        if (error != 0)
        {
            closesocket(z_brd_cfg_pointer->s);
            ZeroMemory(&saddr, sizeof(saddr));
            WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.


            //Меняю статус платы
            if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
            {
                z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
            }

            //ready_to_delete = true;
            z_brd_cfg_pointer->z_thr.z_run = false;
            return;
        }

        z_brd_cfg_pointer->z_thread_status=30;

        if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
        {
            z_brd_cfg_pointer->State = SGDK::DATA::Connected;


            do  // основной цикл thread
            {
                // читаю текущий пакет
                if (SOCKET_ERROR == (actual_len = recv(z_brd_cfg_pointer->s, (char*)&z_brd_cfg_pointer->buff+cursor_pos, len-cursor_pos, 0)))//из сокета s читаю в buff  len байт
                {
                    // Error...
                    error = WSAGetLastError();
                    std::string S = "Thread " + std::to_string(z_brd_cfg_pointer->BrdNomb) + " reading socket Error";
                    //SGDK::DATA::config::write_log(S);
                }
                else  // Чтение прошло успешно
                {
                    if (actual_len + cursor_pos < len)  // пакет получен не полностью
                    {
                        if ( (z_brd_cfg_pointer->buff[0]==0xca) | (z_brd_cfg_pointer->buff[0]==0xcb) )
                        {
                           cursor_pos += actual_len; //пришел фрагмент правильного пакета
                        }
                        else // пришел служебный пакет, игнорю
                        {
                            cursor_pos = 0;
                        }
                    }
                    else
                    {
                        cursor_pos = 0;

                        // Если включена запись фрагмента в bin то пишу пакет
                        write_custom_bin_file();

                        // обрабатываю полученный пакет
                        if ( (z_brd_cfg_pointer->buff[0]==0xca) | (z_brd_cfg_pointer->buff[0]==0xcb) )   // проверка стартового байта
                        {
                            GetTempChanelData();  // чтение данных из температурного канала
                            count_sko(); // подсчет СКО и ДЕЛЬТЫ
                            fill_graph_buff(); // заполняю буфер для рисования графиков

                            if (z_brd_cfg_pointer->metrol_enabled)  // обновляю метрологич. таблицу
                            {
                                // По хорошему надо усреднять или выборку делать реже
                                for (int i=0;i<4;i++)  // данные из 4-х последних пакетов
                                {
                                    z_brd_cfg_pointer->metrol_data[i+12] = z_brd_cfg_pointer->metrol_data[i+8];
                                    z_brd_cfg_pointer->metrol_data[i+8] = z_brd_cfg_pointer->metrol_data[i+4];
                                    z_brd_cfg_pointer->metrol_data[i+4] = z_brd_cfg_pointer->metrol_data[i];

                                }

                                z_brd_cfg_pointer->metrol_data[0] = ( (z_brd_cfg_pointer->buff[7]<<8) + z_brd_cfg_pointer->buff[6] +
                                                         (z_brd_cfg_pointer->buff[15]<<8) + z_brd_cfg_pointer->buff[14] +
                                                         (z_brd_cfg_pointer->buff[23]<<8) + z_brd_cfg_pointer->buff[22] +
                                                         (z_brd_cfg_pointer->buff[31]<<8) + z_brd_cfg_pointer->buff[30] )/4;

                                z_brd_cfg_pointer->metrol_data[1] = ( (z_brd_cfg_pointer->buff[9]<<8) + z_brd_cfg_pointer->buff[8] +
                                                         (z_brd_cfg_pointer->buff[17]<<8) + z_brd_cfg_pointer->buff[16] +
                                                         (z_brd_cfg_pointer->buff[25]<<8) + z_brd_cfg_pointer->buff[24] +
                                                         (z_brd_cfg_pointer->buff[33]<<8) + z_brd_cfg_pointer->buff[32] )/4;

                                z_brd_cfg_pointer->metrol_data[2] = ( (z_brd_cfg_pointer->buff[11]<<8) + z_brd_cfg_pointer->buff[10] +
                                                         (z_brd_cfg_pointer->buff[19]<<8) + z_brd_cfg_pointer->buff[18] +
                                                         (z_brd_cfg_pointer->buff[27]<<8) + z_brd_cfg_pointer->buff[26] +
                                                         (z_brd_cfg_pointer->buff[35]<<8) + z_brd_cfg_pointer->buff[34] )/4;

                                z_brd_cfg_pointer->metrol_data[3] = ( (z_brd_cfg_pointer->buff[13]<<8) + z_brd_cfg_pointer->buff[12] +
                                                         (z_brd_cfg_pointer->buff[21]<<8) + z_brd_cfg_pointer->buff[20] +
                                                         (z_brd_cfg_pointer->buff[29]<<8) + z_brd_cfg_pointer->buff[28] +
                                                         (z_brd_cfg_pointer->buff[37]<<8) + z_brd_cfg_pointer->buff[36] )/4;


                            }


                            // Если включена запись фрагмента CSV то пишу
                            write_custom_csv_file();

                            if (z_brd_cfg_pointer->BoardMode == DATA::BrdModes::test)
                            {
                                pause_time = 0;  // завершаю все расчеты веса и подсчет колес.
                                z_brd_cfg_pointer->isa_connection_status = false; // завершаю поток передачи данных в прогу Исаева
                            }
                            else
                            {
                                // передача на обработку прогой Исаева (real time)
                                if (z_brd_cfg_pointer->isa_real_time)
                                {
                                    sent_data_to_isa_thread();
                                }

                                // ------- определение наезда / съезда -------- //
                                int start_weel_pos = capture_zero(z_brd_cfg_pointer->buff, end_weel);
                                if (start_weel_pos >= 0) // произошел наезд
                                {
                                    pause_time = wait_next_weel;        //
                                }
                                if ( (start_weel_pos >= 0) && (start_capture == 0) ) // произошел наезд
                                {
                                     start_capture = 1;

                                     // Меняю статус платы
                                     if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
                                     {
                                        z_brd_cfg_pointer->State = SGDK::DATA::Train_detection;
                                     }

                                     time_val_prefix = 0;

                                     create_archiv_folder(); // формирую путь для записи проезда

                                     // запускаю новый поток обработки
                                     start_train_obrab_thread();

                                     //пишу в файл и на обработку в объект train_obrab из циклич. буфера
                                     while (Datchik_buf.get_count() > 0) // пока не очистится весь буфер
                                     {
                                         //obrabotka::point d;
                                         data_point d = Datchik_buf.get_point_data();

                                         safe_pushback_data_buf_vector(d);
                                         write_csv(d);
                                         write_isa(d);
                                     }
                                }
                            } // end if (BOARD.BoardMode == DATA::BrdModes::test)

                            if ((start_capture == 1) & (pause_time == 0))
                            {
                                // проезд закончен
                                start_capture = 0;

                                // Меняю статус платы
                                if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
                                {
                                    z_brd_cfg_pointer->State = SGDK::DATA::Connected;
                                }
                                // Закрываю файлы csv и bin для записи проезда
                                if (csv_file.isOpen()) { csv_file.close(); }
                                if (bin_file.isOpen()) { bin_file.close(); }

                                //  ----- фрагмент для запуска программы Исаева в файловом варианте -----
                                if (z_brd_cfg_pointer->write_isa == true)
                                {
                                    if (isa_file.isOpen())
                                    {
                                        isa_file.close();
                                        isa_file_ready = true;
                                    }
                                }
                                /* -------------------------------------------------------  */

                                if (z_brd_cfg_pointer->process)
                                {
                                    train_obrab.stop = true; // сигнал окончания проезда для дочернего потока обработки
                                }
                            }

                            if (pause_time > 0)   // идет запись осцилограммы в csv
                            {
                                // отдаю на обработку и пишу текущий пакет в файлы
                                unsigned int t = (z_brd_cfg_pointer->buff[3]<<8) + (z_brd_cfg_pointer->buff[2]);
                                for (int i=0; i<16; i++)
                                {
                                    data_point dat;
                                    dat.time = (t<<4) + i + (time_val_prefix*1048575);

                                    for (int j=1; j<5; j++)
                                    {
                                        dat.dat[j-1] = (z_brd_cfg_pointer->buff[5+2*j + i*8] << 8) + (z_brd_cfg_pointer->buff[4+2*j + i*8]);
                                    }


                                    safe_pushback_data_buf_vector(dat);
                                    write_isa(dat);
                                    write_data_to_bin();
                                    write_csv(dat);
                                }
                                if(t==0xffff) time_val_prefix++;
                                pause_time--;
                            }
                            else
                            {
                                write_data_to_datchik_buf();  // заполнение циклического буфера
                                write_data_to_temp_bin();

                                // запись значений 0 в файл
                                fill_zero_buff();

                            }
                            // --- периодически обновляю значения АЦП в таблице --- //
                            update_adc_values_in_table();
                        }
                        else
                        {
                            //Запись в лог о битом пакете
                            //std::string S = "Connection to " + APP->pModel->Cfg.BrdConfVector[nomber_in_cfg_list].Ip_str.toStdString() + " failed";
                            //SGDK::DATA::config::write_log(S);
                        }
                    }
                }
            }while( (!error) &&
                   ((z_brd_cfg_pointer->State == SGDK::DATA::Connected) ||
                    (z_brd_cfg_pointer->State == SGDK::DATA::Train_detection)) );      //Если ошибка чтения то выхожу
        }

        z_brd_cfg_pointer->z_thread_status=40;

        // посылаю сигнал для завершения дочернего потока передачи Исаеву
        z_brd_cfg_pointer->isa_connection_status = false;
        isa_rt_thread.isa_force_exit = true;

        closesocket(z_brd_cfg_pointer->s);

        if(error) // произошла ошибка чтения
        {
            // Пишу в лог ошибку связи с платой
            //std::string S = "Connection to " + APP->pModel->Cfg.BrdConfVector[nomber_in_cfg_list].Ip_str.toStdString() + " failed";
            //SGDK::DATA::config::write_log(S);
        }

        z_brd_cfg_pointer->z_thread_status=41;

        // жду остановки потока передачи Исаеву
        wait_until_false(&isa_rt_thread.isa_thread_is_running);

        z_brd_cfg_pointer->z_thread_status=42;

        // меняю статус платы
        if (z_brd_cfg_pointer->State != SGDK::DATA::Off)
        {
            z_brd_cfg_pointer->State = SGDK::DATA::Disconnected;
        }
        else
        {
            // пишу в лог что плату отключили
            //std::string S = "Board " + APP->pModel->Cfg.BrdConfVector[nomber_in_cfg_list].Ip_str.toStdString() + " is off";
            //SGDK::DATA::config::write_log(S);
        }

        ZeroMemory(&saddr, sizeof(saddr));
        WSACleanup();               // Выгружаем DLL-библиотеку из памяти и закрываем поток.

        // останавливаю поток обработки
        train_obrab.stop = true;

        z_brd_cfg_pointer->z_thread_status=43;

        // жду пока остановится
        wait_until_true(&train_obrab.ready_to_delete);

        z_brd_cfg_pointer->z_thread_status=44;

        //ready_to_delete = true;
        z_brd_cfg_pointer->z_thr.z_run = false;
    }

    /* --------------------------------------------------- */

    void Z_thread::write_custom_bin_file()
    {
        if (z_brd_cfg_pointer->write_custom_bin)
        {
            //temp_bin_file.write((const char*)BOARD.buff,len);
            if (z_brd_cfg_pointer->custom_bin_file->isOpen())
            {
                z_brd_cfg_pointer->custom_bin_file->write((const char*)z_brd_cfg_pointer->buff,len);
            }
        }
    }

    /* --------------------------------------------------- */

    void Z_thread::write_custom_csv_file()
    {
        if (z_brd_cfg_pointer->write_custom_csv)
        {
            if (z_brd_cfg_pointer->custom_csv_file->isOpen())
            {
               QTextStream stream(z_brd_cfg_pointer->custom_csv_file);
               int csv_data;
               for (int i=0; i<8; i++)
               {
                   csv_data = (( (z_brd_cfg_pointer->buff[2]<<8) + z_brd_cfg_pointer->buff[3] )<<3) + i; //время
                   stream << csv_data <<";";
                   csv_data = (z_brd_cfg_pointer->buff[7 + 8*i]<<8) + z_brd_cfg_pointer->buff[6 + 8*i];
                   stream << csv_data <<";";
                   csv_data = (z_brd_cfg_pointer->buff[9 + 8*i]<<8) + z_brd_cfg_pointer->buff[8 + 8*i];
                   stream << csv_data <<";";
                   csv_data = (z_brd_cfg_pointer->buff[11 + 8*i]<<8) + z_brd_cfg_pointer->buff[10 + 8*i];
                   stream << csv_data <<";";
                   csv_data = (z_brd_cfg_pointer->buff[13 + 8*i]<<8) + z_brd_cfg_pointer->buff[12 + 8*i];
                   stream << csv_data <<";";
                   stream << "\n";
               }
            }
        }
    }

    /* --------------------------------------------------- */

    void Z_thread::open_csv()
    {
        if (z_brd_cfg_pointer->write_csv == true)
        {
            if (!csv_file.isOpen())
            {
                QString full_file_name = APP->pModel->Cfg.archive_folder + "//" + z_brd_cfg_pointer->csv_file_name;
                csv_file.setFileName(full_file_name);
                csv_file.open(QIODevice::WriteOnly);
            }
        }
    }

/* --------------------------------------------------- */

    void Z_thread::create_archiv_folder()
    {
        if (!APP->pModel->Cfg.archive_folder_created)
        {
            // отчищаю данные предыдущего проезда
            clear_train_data(); // тут гарантируется однократная очистка

            // создаю новую папку для записи проезда
            APP->pModel->Cfg.archive_folder = APP->pModel->Cfg.archiv_dir + get_time_str() ;
            QDir dir(APP->pModel->Cfg.archive_folder);
            if (!dir.exists())
            {
              dir.mkdir(".");
            }

            APP->pModel->Cfg.archive_folder_created = true;
        }
    }

/* --------------------------------------------------- */

    void Z_thread::write_data_to_temp_bin()
    {
        if (z_brd_cfg_pointer->write_bin)
        {
            if (!temp_bin_file.isOpen())
            {
                temp_bin_file.setFileName(temp_bin_file_name1);
                /*bool OK =*/ temp_bin_file.open(QIODevice::WriteOnly);
            }

            if ( temp_bin_size++ > max_temp_bin_size)
            {
                temp_bin_size = 0;
                //удаляю файл №2
                QFile::remove(temp_bin_file_name2);

                //закрываю файл №1
                temp_bin_file.close();

                //переименовываю №1 в №2
                QFile::rename(temp_bin_file_name1, temp_bin_file_name2);

                //открываю для записи новый файл №1
                if (!temp_bin_file.isOpen())
                {
                    temp_bin_file.setFileName(temp_bin_file_name1);
                    temp_bin_file.open(QIODevice::WriteOnly);
                }
            }

            // пишу данные (135 байт в файл)
            temp_bin_file.write((const char*)z_brd_cfg_pointer->buff,len);  // sigserv
        }
    }


/* --------------------------------------------------- */

    void Z_thread::write_data_to_bin()
    {
        if (z_brd_cfg_pointer->write_bin)
        {
            if (!bin_file.isOpen())
            {
                // формирую имя выходного файла
                QString full_file_name = APP->pModel->Cfg.archive_folder
                                + "\\" + z_brd_cfg_pointer->bin_file_name;

                bin_file.setFileName( full_file_name );
                bin_file.open(QIODevice::WriteOnly);

                // закрываю временные бинарники
                temp_bin_file.close();

                // перемещаю все из временных файлов в итоговый бинарник
                copy_from_temp_to_bin(temp_bin_file_name1);
                copy_from_temp_to_bin(temp_bin_file_name2);
            }

            // пишу данные (135 байт в файл)
            bin_file.write((const char*)z_brd_cfg_pointer->buff, len);
        }
    }

/* --------------------------------------------------- */

    void Z_thread::copy_from_temp_to_bin(QString file_name)
    {
        temp_bin_file.setFileName( file_name );
        temp_bin_file.open(QIODevice::ReadOnly);
        if (temp_bin_file.isOpen() && bin_file.isOpen())
        {
            bin_file.write((char*)temp_bin_file.map(0,temp_bin_file.size()), temp_bin_file.size());
        }
        temp_bin_file.close();
        temp_bin_file.remove();
    }

/* --------------------------------------------------- */

    void Z_thread::start_train_obrab_thread()
    {
        if (z_brd_cfg_pointer->process)
        {
            train_obrab.data_buf_vector.clear();
            train_obrab.zero_level = zero_level_old; // передаю в обработку уровень 0
            train_obrab.stop = false; //флаг окончания проезда

            train_obrab.board_obrab_cfg_pointer = z_brd_cfg_pointer;

            train_obrab.start();  // запуск обработки в отд. потоке
        }
    }

    /*------------------------------------------------------------------------------*/

    bool Z_thread::dirExists(const std::string& dirName_in)
    {
      DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
      // -- похоже что лишняя проверка --- //
      //if (ftyp == INVALID_FILE_ATTRIBUTES)
      //  return false;  //something is wrong with your path!
      if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;   // this is a directory!
      return false;    // this is not a directory!
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::update_adc_values_in_table()
    {
        if (update_timer-- == 0)
        {
            update_timer = update_interval;

            // запись значений АЦП
            for(int j=0;j<4;j++)
            {
                z_brd_cfg_pointer->ADC[j]=(z_brd_cfg_pointer->buff[7+j*2]<<8)+z_brd_cfg_pointer->buff[6+j*2];
            }
        }
    }

    /*------------------------------------------------------------------------------*/

    void Z_thread::safe_pushback_data_buf_vector(const data_point &P)
    {
        // безопасное добавление данных в вектор
        wait_until_true(&train_obrab.data_buf_vector_ready_semafor);
        train_obrab.data_buf_vector_ready_semafor = false;
        train_obrab.data_buf_vector.push_back(P);
        train_obrab.data_buf_vector_ready_semafor = true;
    }

/*------------------------------------------------------------------------------*/

    void Z_thread::write_data_to_datchik_buf()
    {
        unsigned int t=(z_brd_cfg_pointer->buff[3] << 8) + (z_brd_cfg_pointer->buff[2]);
        // Запись в циклический буфер
        for (int i=0; i<16; i++)
        {
            //из времени пакета формирую время точки (в пакете 16 точек => сдвиг на 4 бита),
            //если время пакета > 0xFFFF => доб 1 в старший разряд
            unsigned int time_val = t << 4 ;
            data_point P;

            time_val = time_val + i + (time_val_prefix*1048575);

            //Datchik_buf[0].set_int_data(time_val);

            P.time = time_val;

            for(int j=1;j<5;j++)
            {
                //Datchik_buf[j].set_int_data((z_brd_cfg_pointer->buff[5+2*j + i * 8] << 8) + (z_brd_cfg_pointer->buff[4+2*j + i * 8]));

                P.dat[j-1] = (z_brd_cfg_pointer->buff[5+2*j + i * 8] << 8) + (z_brd_cfg_pointer->buff[4+2*j + i * 8]);
            }
        }
        if(t==0xffff) time_val_prefix++;
    }

/*------------------------------------------------------------------------------*/

    void Z_thread::sent_data_to_isa_thread()
    {
        if (z_brd_cfg_pointer->isa_connection_status)
        {
            //соединение установлено, заполняю буфер
            isa_rt_thread.buff[12]= 1;  //№ весов

            for (int i = 0; i < 8; i++)
            {
                for (int j=0; j<2; j++)
                {
                    unsigned int d1 = (z_brd_cfg_pointer->buff[7+4*j+i*8] << 8)+(z_brd_cfg_pointer->buff[6+4*j+i*8]);
                    unsigned int d2 = (z_brd_cfg_pointer->buff[9+4*j+i*8] << 8)+(z_brd_cfg_pointer->buff[8+4*j+i*8]);
                    short ps = (d1+d2)/2;

                    isa_rt_thread.buff[4+2*j]= ps;
                    isa_rt_thread.buff[5+2*j]= ps>>8;
                    isa_rt_thread.buff[8+2*j]= ps;
                    isa_rt_thread.buff[9+2*j]= ps>>8;
                }
                isa_rt_thread.buff_is_ready = true;
                //жду конца передачи
                wait_until_true(&isa_rt_thread.buff_is_ready);
            }
        }
        else
        {
            if(isa_real_time_connection_timer--)
            {
                // Попытка выполнить соединение
                isa_rt_thread.start();
                isa_real_time_connection_timer = max_isa_real_time_connection_timer;
            }
        }
    }

/*------------------------------------------------------------------------------*/
    // перегружена //
    void Z_thread::write_csv(unsigned int arr[], int nomb)
    {
        if (z_brd_cfg_pointer->write_csv)
        {
            if (!csv_file.isOpen())
            {
               QString full_file_name = APP->pModel->Cfg.archive_folder + "//" + z_brd_cfg_pointer->csv_file_name;
               csv_file.setFileName(full_file_name);
               csv_file.open(QIODevice::WriteOnly);
            }
            write_csv(&csv_file, arr, nomb);
        }
    }

    /*------------------------------------------------------------------------------*/
    // перегружена //
    void Z_thread::write_csv(QFile* F, unsigned int arr[], int nomb)  //Файл должен быть открыт заранее
    {
        if (F->isOpen())
        {
            QTextStream stream(F);
            for (int i=0; i<nomb; i++)
            {
                stream << arr[i] <<";";
            }
            stream << "\n";
        }
    }

    void Z_thread::write_csv(data_point P)
    {
        unsigned int dat[5] = {P.time, P.dat[0],P.dat[1],P.dat[2],P.dat[3]};
        write_csv(dat, 5);


    }

/*------------------------------------------------------------------------------*/

    void Z_thread::write_isa(data_point d)
    {
        if (z_brd_cfg_pointer->write_isa)
        {
            if (!isa_file.isOpen())
            {
               QString full_file_name = APP->pModel->Cfg.archive_folder + "//" + z_brd_cfg_pointer->isa_file_name;
               isa_file.setFileName(full_file_name);
               isa_file.open(QIODevice::WriteOnly);
            }

            if (isa_file.isOpen())
            {
                // считаю полусуммы
                short ps1 = (d.dat[0]+d.dat[1])/2 - 0x7FFF;
                short ps2 = (d.dat[2]+d.dat[3])/2 - 0x7FFF;

                isa_file.write((char*)&ps1,sizeof ps1);
                isa_file.write((char*)&ps2,sizeof ps2);
                isa_file.write((char*)&ps1,sizeof ps1);
                isa_file.write((char*)&ps2,sizeof ps2);
            }
        }
    }
/*------------------------------------------------------------------------------*/

    int Z_thread::code_for_isaev_message(short ps1, short ps2)
    {
        // тут реализована половина Исаевской функции,
        // 3 и 4 полусуммы кодируются повторным вызовом

        float koeff = 3.5;
        char znak = 0;
        if      ( (ps1>=0) & (ps2>=0) ) znak = 1;
        else if ( (ps1<0)  & (ps2<0)  ) znak = 2;
        else if ( (ps1>=0) & (ps2<0)  ) znak = 3;
        else if ( (ps1<0)  & (ps2>=0) ) znak = 4;

        int A = round(abs(ps1)/koeff);
        int B = round(abs(ps2)/koeff);
        return znak*100000000 + A*10000 + B;
    }
/*------------------------------------------------------------------------------*/
    int Z_thread::capture_zero(unsigned char* buf, int& last_position)
    {
        // коэфициенты фильтра
        float pp1 = 1.9439882771980;
        float pp2 = -0.94585573006441;
        float kk0 = 0.0009423892225444;
        float kk1 = 0.0009250636437780;

        int delta = 200;     // максимальный шум нуля
        int delta_level = 800;    // порог срабатывания наезда / съезда - потом буду читать из конфига

        float u;
        int rez = -1;

        // Проверка наезда
        if ((zero_level_old_old <= zero_level_old + delta) & (zero_level_old_old + delta >= zero_level_old))      // старый ноль стабилизировался
        {
            for (int i = 0; i < 16; i++)        // цикл по 16 точкам в пакете
            {
                u = ((buf[7+i*8]<<8) + (buf[6+i*8])) + ((buf[9+i*8]<<8) + (buf[8+i*8])) +
                    ((buf[11+i*8]<<8) + (buf[10+i*8])) + ((buf[13+i*8]<<8) + (buf[12+i*8]));

                if (std::abs(u-zero_level_old) >= delta_level )  // сработал наезд
                {
                    last_position = 100;         // сброс съезда
                    if (rez == -1) rez = i;
                }
                else    // Съезд
                {
                    if (last_position>16)  last_position = i;
                }
            }
        }
        if (rez == -1)           // Если нет наезда, то пишу новый ноль в кольцевой буфер
        {
            u=((buf[7]<<8)+(buf[6])) + ((buf[9]<<8)+(buf[8])) +
              ((buf[11]<<8)+(buf[10])) + ((buf[13]<<8)+(buf[12])); // вход фильтра - сумма по 4 датчикам

            // начальная инициализация фильтра 0
            float xx1_1;
            if (teta1_1_2 == 0) teta1_1_2 = u;
            if (teta1_1_1 == 0) teta1_1_1 = u;
            if (u1_1 == 0) u1_1 = u;
            //if (xx1_1 == 0) xx1_1 = u;

            //  фильтр нуля
            xx1_1 = pp1 * teta1_1_1 + pp2 * teta1_1_2 + kk0 * u + kk1 * u1_1;    // // текущий результат фильтрации
            teta1_1_2 = teta1_1_1;
            teta1_1_1 = xx1_1;
            u1_1 = u;

            // Добавляю в кольцевой буфер значение
            zero_ring_buff.set_float_data(xx1_1);

            // Если буфер заполнен, то извлекаю самое старое значение
            if (zero_ring_buff.get_count() == zero_ring_buff.get_max_len()-1)
            {
                zero_level_old_old = zero_level_old;
                zero_level_old = zero_ring_buff.get_float_data();   // самый дальний ноль
            }
        }
        return rez;
    };
/*------------------------------------------------------------------------------*/

}   //namespace SGDK
