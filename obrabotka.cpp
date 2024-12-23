#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include "obrabotka.h"
#include "ring_buf.h"
#include "Matrix.h"
#include "data.h"
#include "application.h"
#include "wait.h"
namespace SGDK
{
    obrabotka::obrabotka() { };

/*-------------------------------------------------------------------*/

    int obrabotka::find_zero_point(int &interval_begin, int &interval_end, int sum_number)
    {   // sum_number - № полусуммы - 0 или 1

        int zero_level_pozition = 0;
        int min_val = 10000;

        if(interval_begin>interval_end)
        {
            int i = interval_begin;
            interval_begin = interval_end;
            interval_end = i;
        }

        for (int i=interval_begin; i<interval_begin; i++)
        {
            int delta = abs(wheel_data[i].dat[2*sum_number] + wheel_data[i].dat[1+2*sum_number] - zero_level);
            if ( delta<min_val )
            {
                min_val = delta;
                zero_level_pozition = i;
            }
        }
        return zero_level_pozition;
    }

/*-------------------------------------------------------------------*/

    double obrabotka::calc_speed_zero_method()
    {
        // определяю абсолютный мин и макс

        int max1=0;
        int max1_poz=0;
        int min1=100000;
        int min1_poz = 0;

        int max2=0;
        int max2_poz=0;
        int min2=100000;
        int min2_poz = 0;

        for (int i=1; i<wh_calc[wheel_nomb].osc_size; i++)
        {
            int sum = wheel_data[i].dat[0]+wheel_data[i].dat[1];
            if (sum>max1)
            {
                max1=sum;
                max1_poz = i;
            }
            if (sum<min1)
            {
                min1=sum;
                min1_poz = i;
            }

            sum = wheel_data[i].dat[2]+wheel_data[i].dat[3];
            if (sum>max2)
            {
                max2=sum;
                max2_poz = i;
            }
            if (sum<min2)
            {
                min2=sum;
                min2_poz = i;
            }
        }

        // Определяю точки пересечения графика с 0 между min и max
        int zero_level_pozition1 = find_zero_point(max1_poz, min1_poz, 0);
        int zero_level_pozition2 = find_zero_point(max2_poz, min2_poz, 1);

        // Определяю скорость по расстоянию между нулями
        double speed =  board_obrab_cfg_pointer->datchik_distance *7.2
                        / abs(zero_level_pozition2 - zero_level_pozition1);

        speed = speed*2 ;  // для 4 кгц эмуляции

        return speed;
    };

/*-------------------------------------------------------------*/

    double obrabotka::calc_speed_isaev()
    {
        double speed = 0;

        int datchik_distance = board_obrab_cfg_pointer->datchik_distance;

        speed = datchik_distance * 7.2 / (wheel_data.size()-1);

        if((xmax<diap_coeff)|(xmax>wheel_data.size()-diap_coeff))
        {
            xmax = wheel_data.size()/2;

            // скорость возможно лучше считать по предыдущему колесу
            if (prev_speed_0 > 0)  speed = prev_speed_0;

            // запись в лог колеса, что колесо не очень хорошее
            wh_calc[wheel_nomb].square_aproxim_error = true;
        }

        speed = speed*2 ;  // для 4 кгц эмуляции

        // Корректирую скорость
        if (prev_speed_0 > 0)
        {
            if ( (speed >= prev_speed_0*2) /*|  (speed_0 <= prev_speed_0/2)*/ )
            {
                // Скорость вычислена неправильно из-за удара
                speed = prev_speed_0;
            }
        }
        return speed;
    }

/*-------------------------------------------------------------*/

    void obrabotka::calculate_stopped_wheel()
    {
        // определяю максимум суммы датчиков

        int sum_dat = 0;
        int max = 0;

        int siz = wheel_data.size();
        for (int i=1; i<siz; i++)
        {
            sum_dat = wheel_data[i].summ_all();
            if (sum_dat>max) max = sum_dat;
        }

        //max * RKP + темпер_поправка = вес
        wh_calc[wheel_nomb].weigth = (max-zero_level) * board_obrab_cfg_pointer->kvant_to_weigth;
    }

/*-------------------------------------------------------------*/

    void obrabotka::calculate_wheel()
    {
        // пока для тестирования пишу колесо в CSV
        write_wheel_to_csv("orig");

        // создаю объект лога колеса
        wheel_calculation new_wheel;
        new_wheel.wheel_nomb = wheel_nomb;
        // добавляем запись в вектор
        try
        {
           wh_calc.push_back(new_wheel);
        }
        catch (std::bad_alloc)
        {
            // запись в лог о невозможности выделить память
            std::string S = "Obrab " + std::to_string(board_obrab_cfg_pointer->BrdNomb) + " wh_calc(146) alloc error";
            SGDK::DATA::config::write_log_all(S);
        }

        wh_calc[wheel_nomb].stopped_wheel = wh_stopped;
        wh_stopped = false;  // сброс флага для след колеса

        wh_calc[wheel_nomb].osc_size = wheel_data.size();
        wh_calc[wheel_nomb].zero_level = zero_level;

        trim_wheel_data(); // обрезаю осцилограмму с 2-х сторон по уровню доли

        // 4 - запись уточненной осцилограммы в csv
        write_wheel_to_csv("trim");

        wh_calc[wheel_nomb].trimmed_osc_size = wheel_data.size();

        // если колесо без остановки, то обрабатываю стандартным способом
        if (wh_calc[wheel_nomb].stopped_wheel)
        {
            calculate_stopped_wheel();
        }
        else
        {
            calculate_nonstop_wheel();
        }
        clear_wheel_data();
    }

/*-------------------------------------------------------------*/

    void obrabotka::calculate_nonstop_wheel()
    {
        // апроксимация параболой для нах-я центрального коэфф
        square_aproxim(xmax,ymax);

        wh_calc[wheel_nomb].square_aproxim_xmax = xmax;
        wh_calc[wheel_nomb].square_aproxim_ymax = ymax;

        wh_calc[wheel_nomb].Koeffic[0] = Koeffic[0];
        wh_calc[wheel_nomb].Koeffic[1] = Koeffic[1];
        wh_calc[wheel_nomb].Koeffic[2] = Koeffic[2];

        // определяю скорость
        double speed_0;
        speed_0 = calc_speed_isaev();
        //speed_0 = calc_speed_zero_method();
        wh_calc[wheel_nomb].speed0 = speed_0;

        // проверка того, как прошла апроксимация,
        // если центр параболы за пределами диапазона, то игнорирую рез-т
        // апроксимации и беру за основу дальнейших расчетов центр осциллограммы
        diap_coeff = wheel_data.size()/10;

        wheel_wait_timer_max = std::round(3000/speed_0); // чем меньше скорость тем дольше жду

        wh_calc[wheel_nomb].xmax = xmax; // в лог уточненное знач xmax

        // определяю время проезда
        wh_calc[wheel_nomb].wheel_time = wheel_data[std::round(wh_calc[wheel_nomb].xmax)].time;
        wh_calc[wheel_nomb].wheel_start_time = wheel_data[0].time;
        wh_calc[wheel_nomb].wheel_end_time = wheel_data[wheel_data.size()-1].time;

        if(prev_wheel_time>0)  // если колесо не первое
        {
            // определяю расстояние между осями и корректирую скорость
            wh_calc[wheel_nomb].prev_distance = wh_calc[wheel_nomb].speed0
                                                * (wh_calc[wheel_nomb].wheel_time
                                                - prev_wheel_time)/7.2;

            wh_calc[wheel_nomb].prev_distance=wh_calc[wheel_nomb].prev_distance /2;  // Для 4 кгц эмуляции
        }
        else wh_calc[wheel_nomb].prev_distance = 0;

        prev_wheel_time = wh_calc[wheel_nomb].wheel_time;
        prev_speed_0 = wh_calc[wheel_nomb].speed0;

        std::string S = "Wheel_" + std::to_string(wheel_nomb) +
                        " Speed = " + std::to_string(speed_0) +
                        " dist = " + std::to_string(wh_calc[wheel_nomb].prev_distance) +
                        " Time = " + std::to_string(wh_calc[wheel_nomb].wheel_time);
        SGDK::DATA::config::write_log_file(S);

        // Расчет веса ( вычисляю коэффициенты апроксимации a и b )
        aproximate(a,b);

        wh_calc[wheel_nomb].weigth = b * board_obrab_cfg_pointer->kvant_to_weigth;

        write_aprox_csv();  //запись колеса с апроксимацией в csv

    }

/*-------------------------------------------------------------------*/

    // ----- сегментация -- Метод Андреича ------ //
    void obrabotka::segmentate_andreich()
    {
        if (cart_wheel_count_stop == 0)
        {
            // 1) Если после последнего вагона проехало 2-е колесо и межколесное расстояние < 2000
            // то это длина тележки
            if ( (wheel_nomb - prev_vag_last_wheel_nomb) == 2)
            {
                if ( (wh_calc[wheel_nomb].prev_distance > 1850-350)
                         & (wh_calc[wheel_nomb].prev_distance < 2850+350))
                {
                    // обнаружена тележка, определяю ее длину
                    cart_len=wh_calc[wheel_nomb].prev_distance;
                    cart_wheel_nomb=2;
                }
                else
                {
                    // одноосная тележка
                    cart_len=0;
                    cart_wheel_count_stop = 1;
                    cart_wheel_nomb=1;
                }
            }

            // 2) Если 3+ колесо на таком-же расстоянии, как и второе, то зто многоосная тележка
            if ( ((wheel_nomb - prev_vag_last_wheel_nomb) > 2) &
                 ((wheel_nomb - prev_vag_last_wheel_nomb) < 4) )
            {
                if ( (wh_calc[wheel_nomb].prev_distance > cart_len-350)
                         & (wh_calc[wheel_nomb].prev_distance < cart_len+350))
                {  // найдено еще 1 колесо в тележке
                    cart_wheel_nomb++;
                }
                else // тележка закончилась
                {
                    cart_distance = wh_calc[wheel_nomb].prev_distance;
                    cart_wheel_count_stop = 1;
                }
            }
        }

        if (cart_wheel_count_stop == 1)   // 2-я и далее тележки
        {
            int wn = wheel_nomb - prev_vag_last_wheel_nomb;
            if ( (wn == cart_wheel_nomb*2+1)
               | (wn == cart_wheel_nomb*3+1) ) // проверяю не сцепка ли зто
            {
                if ( (wh_calc[wheel_nomb].prev_distance > 2000)
                      & (wh_calc[wheel_nomb].prev_distance < 6100))
                {
                    // Сцепка
                    cart_wheel_count_stop = 0;
                    cart_distance = 0;
                    cart_wheel_nomb = 0;

                    // найден промежуток между вагонами
                    // помечаю все оси между prev_vag_last_wheel_nomb и текущей
                    // как принадлежащие одному вагону
                    for (int i = prev_vag_last_wheel_nomb+1; i<wheel_nomb; i++)
                    {
                        wh_calc[i].van_nomb = van_nomb;
                        wh_calc[i].van_type = 1;

                        wheel_to_screen(i);  // вывод на экран
                    }
                    prev_vag_last_wheel_nomb = wheel_nomb-1;
                    van_nomb++;
                }
            }
        }
    }  // ! Сегментация

    /*-------------------------------------------------------------------*/

    // Упрощеный метод сегментации
    void obrabotka::segmentate_simple()
    {
        if ( (wheel_nomb - prev_vag_last_wheel_nomb) >= 5 )
        {
            if ( (wh_calc[wheel_nomb].prev_distance > 2350-250)
                      & (wh_calc[wheel_nomb].prev_distance < 2350+1200)
                      & (wh_calc[wheel_nomb-1].prev_distance > 1850-400)
                      & (wh_calc[wheel_nomb-1].prev_distance < 1850+400) )
            {   // найден промежуток между вагонами
                // помечаю все оси между prev_vag_last_wheel_nomb и текущей
                // как принадлежащие одному вагону
                for (int i = prev_vag_last_wheel_nomb+1; i<wheel_nomb; i++)
                {
                    wh_calc[i].van_nomb = van_nomb;
                    wh_calc[i].van_type = 1;

                    // вывод на экран
                    wheel_to_screen(i);
                }
                prev_vag_last_wheel_nomb = wheel_nomb-1;
                van_nomb++;
            }
            else // проверка на локомотив
            {
               if ( (wh_calc[wheel_nomb].prev_distance > 5000-1100)
                  & (wh_calc[wheel_nomb].prev_distance < 5000+1100)
                  & (wh_calc[wheel_nomb-1].prev_distance > 2850-300)
                  & (wh_calc[wheel_nomb-1].prev_distance < 2850+300) )
               {   // найден промежуток между вагонами
                   // помечаю все оси между prev_vag_last_wheel_nomb и текущей
                   // как принадлежащие одному вагону
                   for (int i = prev_vag_last_wheel_nomb+1; i<wheel_nomb; i++)
                   {
                       wh_calc[i].van_nomb = van_nomb;
                       wh_calc[i].van_type = -1;

                       // вывод на экран
                       wheel_to_screen(i);
                   }
                   prev_vag_last_wheel_nomb = wheel_nomb-1;
                   van_nomb++;
                }
             }
         }
    }

/*-------------------------------------------------------------------*/

    void obrabotka::run()
    {
        //*----- log -----*/     std::string flname = "obr_err_" + std::to_string(board_id) + ".log";
        //*----- log -----*/     obr_err_log_file.open(flname.c_str());

        ready_to_delete = false;

        capture_wheel_started = false;
        endwheel = false;

        clear_wheel_data();
        wheel_nomb  = 0;
        prev_wheel_time=0;

        board_obrab_cfg_pointer->train_started = true;

        wh_calc.clear(); // очистка структуры инфы о всех колесах

        prev_vag_last_wheel_nomb = -1; // № последнего колеса пред вагона
        van_nomb = 0;

        do
        {
            //надо добавить защиту от зависания, если слишком долго в цикле, то выход
            while(data_buf_vector.size()>0 ) // есть что обрабатывать
            {
                //получаю данные из буфера
                data_point point_dat = data_buf_vector[0];
                erase_point_from_data_buf_vector();

                if (capture_wheel_started)
                {
                    //пишу в вектор все колесо
                    //for (int j=0;j<5;j++)
                    {
                        wheel_data.push_back(point_dat);
                    }

                    //проверяю нет ли съезда
                    detect_wheel_end(point_dat.summ_all());
                }
                else
                {
                    // пишу в pre_wheel_buf
                    //for (int j=0;j<5;j++)
                    {
                        pre_wheel_buf.set_point_data(point_dat);
                    }

                    //определяю нет ли наезда
                    int sum_dat = point_dat.summ_all();

                    if( sum_dat-zero_level > delta_level )
                    {
                        //выставляю флаг capture_wheel_started
                        //и переношу данные из pre_wheel_buf в wheel_data
                        start_capture();
                    }
                }

                if(endwheel) // колесо полностью в буфере, можно обрабатывать
                {
                /*///  =========== !!! ОБРАБОТКА КОЛЕСА !!! ==========  ///*/
                    // осцилл колеса находится в std::vector<int> wheel_data[5]; [0] - время в тиках

                    calculate_wheel();

                    // разбивка на вагоны
                    // prev_vag_last_wheel_nomb -  номер последнего колеса предыдущего вагона
                    // wh_calc[i] - данные проезда i-го колеса

                    //segmentate_andreich();  //-- сегментация - Метод Андреича --//
                    segmentate_simple();  //-- Упрощенный метод сегментации --//

                    endwheel= false;
                    wheel_nomb++;
                }
            }
        }
        while(!stop);  // если буфер пуст, но проезд не завершен

        // проезд завершен

        // ставлю № последнего вагона
        for (int i = prev_vag_last_wheel_nomb+1; i<wheel_nomb; i++)
        {
            wh_calc[i].van_nomb = van_nomb;
            // вывод на экран
            wheel_to_screen(i);
        }

        save_wheels_log();
        stop = false;
        //*----- log -----*/   obr_err_log_file.close();

        board_obrab_cfg_pointer->train_stopped = true; //для проверки в mainwindow что все платы завершили обработку
        ready_to_delete = true;
    };

/*------------------------------------------------------------------------------------*/
    obrabotka::~obrabotka()
    {
        stop = true;
        wait_until_true(&ready_to_delete);
    }


    void obrabotka::erase_point_from_data_buf_vector()
    {
        // проверяю не делает ли другой поток операции с этим вектором
        wait_until_true(&data_buf_vector_ready_semafor);
        data_buf_vector_ready_semafor = false; // блокирую
        if (data_buf_vector.size()>0)
        {
            data_buf_vector.erase(data_buf_vector.begin());
        }
        data_buf_vector_ready_semafor = true; // разблокирую
    }

/*------------------------------------------------------------------------------------*/

    void obrabotka::detect_wheel_end(int sum_level)
    {
        if( sum_level-zero_level > delta_level )
        {
            // съезда нет или после короткого провала снова уровень поднялся
            // сбрасываю счетчик ожидания правильного съезда
            end_wheel_timer = wheel_wait_timer_max;
        }
        else // уровень сигнала упал ниже порога, отсчитываю несколько точек и если не повысится, то съезд
        {
            end_wheel_timer--;
            if (end_wheel_timer == 0)
            {
                // произошел гарантированный съезд
                end_wheel_timer = wheel_wait_timer_max;
                capture_wheel_started = false;
                endwheel = true;
            }
        }
    }

/*------------------------------------------------------------------------------------*/

    void obrabotka::start_capture()
    {
        capture_wheel_started =true;

        //пишу в вектор содержимое буфера
        while (pre_wheel_buf.get_count()>0)
        {
            //for (int j=0;j<5;j++)
            {
                wheel_data.push_back(pre_wheel_buf.get_point_data());
            }
        }
    }

 /*------------------------------------------------------------------------------------*/

    void obrabotka::wheel_to_screen(int w_n)
    {
        // вывод на экран (пишу в буфер
        //а потом в mainwindow достаю из буфера и объединяю
        wheel_rez_data dat;
        dat.wheel_rez_data_brd_cfg_pointer = board_obrab_cfg_pointer;
        dat.van_nomb = wh_calc[w_n].van_nomb;
        //dat.side = APP->pModel->Cfg.BrdConfVector[board_id].side;
        if(board_obrab_cfg_pointer->side == 0)
        {
            dat.weigth_r = wh_calc[w_n].weigth;
            dat.weigth_l = 0;
        }
        else
        {
            dat.weigth_l = wh_calc[w_n].weigth;
            dat.weigth_r = 0;
        }
        dat.wheel_nomb = wh_calc[w_n].wheel_nomb;
        APP->wheels_buff.push_back(dat);
    }

/*------------------------------------------------------------------------------------*/

    // апроксимация функцией power_function()
    void obrabotka::aproximate(double &a,double &b)
    {
        // сдвигаю ось X на xmax и апроксимирую функцией Ax^n + B
        // n задается в конфиге (power)
        // power_function - возвращает число в нужной степени
        double sumxn  = 0;
        double sumxny = 0;
        double sumx2n = 0;
        double sumy   = 0;

        for(unsigned int i=0; i<wheel_data.size(); i++)
        {
            double sum = wheel_data[i].summ_all() - zero_level;
            sumxn  += power_function( abs(i-xmax) );  //сдвигаю ось X на xmax
            sumxny += power_function( abs(i-xmax) )*sum;
            sumx2n += power_function( abs(i-xmax) )*power_function( abs(i-xmax) );
            sumy   += sum;
        }

        // вычисляю коэффициенты a и b
        b = -(sumy*sumx2n - sumxn*sumxny)/(sumxn*sumxn - wheel_data.size()*sumx2n);
        wh_calc[wheel_nomb].b = b;

        a = (sumy-b*wheel_data.size())/sumxn;
        //a = (sumxny-sumxn*sumy)/(sumxn*sumxn - sumx2n);
        wh_calc[wheel_nomb].a = a;
    }

/*------------------------------------------------------------------------------------*/

    void obrabotka::create_wheel_folder()
    {
        //создаю папку wheels
        QString wheels_dir_name = APP->pModel->Cfg.archive_folder + "//wheels" ;
        QDir dir(wheels_dir_name);
        if (!dir.exists())
        {
          dir.mkdir(".");
        }
    }

    /* ---------------------------------------------------------------------*/

    void obrabotka::write_aprox_csv()
    {
        if (board_obrab_cfg_pointer->write_wheel)
        {
            create_wheel_folder();

            QString csv_file_name = APP->pModel->Cfg.archive_folder +
                                  "/wheels/brd_" + QString::number(board_obrab_cfg_pointer->BrdNomb) +
                                  "port_" + QString::number(board_obrab_cfg_pointer->port) +
                                  "_wh_" + QString::number(wheel_nomb) +
                                  "aprox.csv";

            QFile wheel_csv_file;
            wheel_csv_file.setFileName(csv_file_name);
            wheel_csv_file.open(QIODevice::WriteOnly);
            QTextStream stream(&wheel_csv_file);

            for(unsigned int i=0; i<wheel_data.size(); i++)
            {

               stream//<< std::to_string(wheel_data[0][i]).c_str() << ";"
                             << std::to_string(wheel_data[i].summ_all()).c_str() << ";"
                             << std::to_string( zero_level + (int)std::round( Koeffic[0]*(i)*(i)+Koeffic[1]*(i)+Koeffic[2] ) ).c_str() << ";"
                             << std::to_string( (int)std::round(a*power_function(i-xmax) + b + zero_level) ).c_str() << ";"
                             << "\n";
            }
            wheel_csv_file.close();
        }
    }

/*------------------------------------------------------------------------------------*/

    void obrabotka::save_wheels_log()
    {

        QString train_log_file_name = APP->pModel->Cfg.archive_folder +
                        "/brd_" + QString::number(board_obrab_cfg_pointer->BrdNomb) +
                        "port_" + QString::number(board_obrab_cfg_pointer->port)  + "_train.log";
        QFile w_log_file ;

        w_log_file.setFileName(train_log_file_name);
        w_log_file.open(QIODevice::WriteOnly);
        QTextStream s(&w_log_file);

        for(unsigned int i=0; i<wh_calc.size(); i++)
        {
            s << "----------------------------" << "\n";
            s << "board      = " << QString::number(board_obrab_cfg_pointer->BrdNomb) << "\n";
            s << "van_nomb   = " << QString::number(wh_calc[i].van_nomb) << "\n";
            s << "van_type   = " << QString::number(wh_calc[i].van_type) << "\n";

            s << "wheel_nomb = " << QString::number(wh_calc[i].wheel_nomb) << "\n";

            s << "stopped_wheel = " << ( (wh_calc[i].stopped_wheel) ? "TRUE" : "FALSE" );

            s << "osc_size = " << QString::number(wh_calc[i].osc_size) << "\n";
            s << "trim_dolja = " << QString::number(wh_calc[i].trim_dolja) << "\n";
            s << "trim_level = " << QString::number(wh_calc[i].trim_level) << "\n";
            s << "koeffic a  = " << QString::number(wh_calc[i].Koeffic[0]) << "\n";
            s << "koeffic b  = " << QString::number(wh_calc[i].Koeffic[1]) << "\n";
            s << "koeffic c  = " << QString::number(wh_calc[i].Koeffic[2]) << "\n";

            s << "max_x = " << QString::number(wh_calc[i].osc_max_x) << "\n";
            s << "max_y = " << QString::number(wh_calc[i].osc_max_y) << "\n";
            s << "trim_l_poz = " << QString::number(wh_calc[i].trim_l_poz) << "\n";
            s << "trim_r_poz = " << QString::number(wh_calc[i].trim_r_poz) << "\n";
            s << "trimmed_osc_size = " << QString::number(wh_calc[i].trimmed_osc_size) << "\n";
            s << "direction  = " << QString::number(wh_calc[i].direction) << "\n";
            s << "zero_level = " << QString::number(wh_calc[i].zero_level) << "\n";
            s << "square aprox xmax = " << QString::number(wh_calc[i].square_aproxim_xmax) << "\n";
            s << "square aprox ymax = " << QString::number(wh_calc[i].square_aproxim_ymax) << "\n";
            s << "xmax   = " << QString::number(wh_calc[i].xmax) << "\n";
            s << "ymax   = " << QString::number(wh_calc[i].ymax) << "\n";
            s << "square aprox err = " << QString::number(wh_calc[i].square_aproxim_error) << "\n";
            s << "wheel time  = " << QString::number(wh_calc[i].wheel_time) << "\n";
            s << "wheel start = " << QString::number(wh_calc[i].wheel_start_time) << "\n";
            s << "wheel_end   = " << QString::number(wh_calc[i].wheel_end_time) << "\n";
            s << "speed 0   = " << QString::number(wh_calc[i].speed0) << "\n";
            s << "speed cor = " << QString::number(wh_calc[i].speed_corr) << "\n";
            s << "prev dist = " << QString::number(wh_calc[i].prev_distance) << "\n";
            s << "corr prev dist = " << QString::number(wh_calc[i].corr_prev_distance) << "\n";
            s << "a  = " << QString::number(wh_calc[i].a) << "\n";
            s << "b  = " << QString::number(wh_calc[i].b) << "\n";

            s << "weigth = " << QString::number(wh_calc[i].weigth) << "\n";
        }
        w_log_file.close();
    }

/*--------------------------------------------------------------------------------------*/

    int obrabotka::calc_dolja_to_trim_wheel()
    {
        int datchik_distance = board_obrab_cfg_pointer->datchik_distance;

        if      (datchik_distance<=250)  return 30;
        else if (datchik_distance<=350)  return 40;
        else if (datchik_distance<=500)  return 50;
        else                             return 60;
    }

/*--------------------------------------------------------------------------------------*/

    // обрезаю осцилограмму с 2-х сторон по уровню доли, результат в vector<> corrected_wheel_data[5]
    void obrabotka::trim_wheel_data()
    {
        // Уточняю диапазон
        // 1 - нахожу максимум
        int max=0;
        int max_poz=0;
        //int siz = wheel_data[0].size();
        int siz = wh_calc[wheel_nomb].osc_size;
        for (int i=1; i<=siz-1; i++)
        {
            int sum = wheel_data[i].summ_all();
            if (sum>max)
            {
                max=sum;
                max_poz = i;
            }
        }

        wh_calc[wheel_nomb].osc_max_x = max_poz;
        wh_calc[wheel_nomb].osc_max_y = max;

        // 2 - нахожу долю в зависимости от расстояния между датчиками
        int dolja = calc_dolja_to_trim_wheel();          // доля в %, т.е. надо /100

        // определяю границы диапазона
        int r_poz = 0;
        int l_poz = 0;

        int level = zero_level + (max - zero_level)*dolja/100;
        wh_calc[wheel_nomb].trim_dolja = dolja;
        wh_calc[wheel_nomb].trim_level = level;

        for (int i=siz-1; i>max_poz; i--) // определяю правую границу диапазона
        {
            int sum = wheel_data[i].summ_all();
            if (sum>=level)
            {
                r_poz = i;
                break;
            }
        }

        wh_calc[wheel_nomb].trim_r_poz = r_poz;

        // Обрезаю вектор справа
        if( r_poz < siz)  // есть что обрезать
        {
            //for (int i=0; i<5; i++)
            {
                auto iter = wheel_data.cend(); // указатель на последний элемент
                wheel_data.erase(iter - wheel_data.size() + r_poz-1, iter);
            }
        }

        long int sum_sum1 = 0;
        long int sum_sum2 = 0;
        for (int i=0; i<=max_poz; i++) // определяю левую границу диапазона
        {
            int sub_sum12 = wheel_data[i].dat[0]+wheel_data[i].dat[1];
            int sub_sum34 = wheel_data[i].dat[2]+wheel_data[i].dat[3];
            sum_sum1 += sub_sum12;
            sum_sum2 += sub_sum34;
            if ( (sub_sum12+sub_sum34)>=level )
            {
                if(sum_sum1>sum_sum2)
                {
                    wh_calc[wheel_nomb].direction= true;
                }
                else
                {
                    wh_calc[wheel_nomb].direction= false;
                }
                l_poz = i;
                break;
            }
        }
        wh_calc[wheel_nomb].trim_l_poz = l_poz;

        // Обрезаю вектор слева
        if( l_poz > 0 )
        {
            //for(int i=0; i<5; i++)
            {
                auto iter = wheel_data.cbegin();
                wheel_data.erase(iter,iter+l_poz);
            }
        }
    }

/*----------------------------------------------------------------*/

    void obrabotka::square_aproxim(double &xmax, double& ymax)
    {
        double sumx4  =0;
        double sumx3  =0;
        double sumx2  =0;
        double sumx   =0;
        double sumx2y =0;
        double sumxy  =0;
        double sumy   =0;
        for(unsigned int i=0; i<wheel_data.size(); i++)
        {
            double sum = wheel_data[i].summ_all()  - zero_level;
            sumx4 += (i)*(i)*(i)*(i);
            sumx3 += (i)*(i)*(i);
            sumx2 += (i)*(i);
            sumx  += (i);
            sumx2y+= (i)*(i)*sum;
            sumxy += (i)*sum;
            sumy  += sum;
        }

        // Метод Крамера решения систем линейных уравнений
        {
          SquareMatrix MainMatrix(3);
          double FreeMembers[3];

          //double aaa =  wheel_data[0].size();
          // заполняю матрицу
          MainMatrix(0,0) = sumx4;  MainMatrix(0,1) = sumx3;   MainMatrix(0,2) = sumx2;
          MainMatrix(1,0) = sumx3;  MainMatrix(1,1) = sumx2;   MainMatrix(1,2) = sumx;
          MainMatrix(2,0) = sumx2;  MainMatrix(2,1) = sumx;   MainMatrix(2,2) = wheel_data.size();

          //заполняю столбец свободных членов
          FreeMembers[0] = sumx2y;
          FreeMembers[1] = sumxy;
          FreeMembers[2] = sumy;

          MainMatrix.Kramer(FreeMembers,Koeffic);
        }
        xmax = -Koeffic[1]/(2*Koeffic[0]);          // -b/(2*a);
        ymax = Koeffic[2] - Koeffic[1]*Koeffic[1]/(4*Koeffic[0]);  //c-b*b/(4*a);
    }

/*-------------------------------------------------------------*/

    double obrabotka::power_function(double X)
    {
        switch (board_obrab_cfg_pointer->power)
        {
            case 20:  return X*X;
            case 25:  return X*X*std::sqrt(std::abs(X));
            case 30:  return std::abs(X*X*X);
            case 35:  return std::abs(X*X*X)*std::sqrt(std::abs(X));
        }
        return X*X;  //по умолчанию
    }

    /*---------------------------------------------------------------*/

    void obrabotka::clear_wheel_data()
    {
        pre_wheel_buf.clear();
        wheel_data.clear();

    }

    /*-------------------------------------------------------------------*/

    void obrabotka::write_wheel_to_csv(QString FileNameSuffix)
    {
        if (board_obrab_cfg_pointer->write_wheel)
        {
            create_wheel_folder();

            QString file_name = APP->pModel->Cfg.archive_folder +
                                "/wheels/brd_" + QString::number(board_obrab_cfg_pointer->BrdNomb) +
                                "port_" + QString::number(board_obrab_cfg_pointer->port)  +
                                "_wh_" + QString::number(wheel_nomb) +
                                FileNameSuffix + ".csv";
            QFile wheel_csv_file;
            wheel_csv_file.setFileName(file_name);
            wheel_csv_file.open(QIODevice::WriteOnly);

            QTextStream s(&wheel_csv_file);
            for(unsigned int i=0; i<wheel_data.size(); i++)
            {
               s<< QString::number(wheel_data[i].time) << ";"
                << QString::number(wheel_data[i].dat[0]) << ";"
                << QString::number(wheel_data[i].dat[1]) << ";"
                << QString::number(wheel_data[i].dat[2]) << ";"
                << QString::number(wheel_data[i].dat[3]) << ";"
  /* сумма */   << QString::number( wheel_data[i].summ_all() ) << "\n";
            }
            wheel_csv_file.close();
        }
    }

    /*-------------------------------------------------------------------*/


} // #namespace SGDK
