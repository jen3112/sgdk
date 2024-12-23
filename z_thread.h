#ifndef Z_THREAD_H
#define Z_THREAD_H

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <QThread>
#include <QFile>
#include <fstream>
#include <string>
#include <QTextStream>
#include "ring_buf.h"
#include "obrabotka.h"
#include "isa_real_time.h"

namespace SGDK
{

namespace DATA
{
    class BoardCfg;
}

    class Z_thread : public QThread
    {
    public:
        WSADATA ws;

        QString temp_bin_file_name1;
        QString temp_bin_file_name2;
        QString zero_file_name;
        QFile csv_file ;
        QFile isa_file ;
        QFile temp_bin_file ;
        QFile bin_file ;

        static const int len = 135;  // длина пакета

        DATA::BoardCfg *z_brd_cfg_pointer;

        unsigned short zeroes_buff[10][8];
        int zero_buff_pos = 0;
        int zero_interval_counter = 0;
        void fill_zero_buff();
        void write_zero_buff_to_file();

        isa_real_time isa_rt_thread;
        sockaddr_in saddr;      // Объявим переменные для хранения адреса

        // создаю статически, мне он нужен всегда 1 на каждый экземпляр захвата
        obrabotka train_obrab;

        bool SetSocketBlockingEnabled(int fd, bool blocking);

        int error = 0;

        int  temp_bin_size = 0;
        int  max_temp_bin_size = 1000; // 1000 пакетов в файле
        int max_isa_real_time_connection_timer = 100;
        int isa_real_time_connection_timer = max_isa_real_time_connection_timer;
        int timeout = 1000;   // установка таймаута для записи в сокет
        int time_val_prefix = 0;
        int start_capture = 0;
        long start_time = 0;
        long stop_time = 0;
        int end_weel = 0;

        int end_weel_delay = 0; // После проеэда сохраняю еще 3 пакета
        int wheel_count = 0;
        const short int wait_next_weel = 500;         // сколько пакетов должно пройти для прекращения записи осцилограммы
        int pause_time = 0;
        int actual_len = 0;
        int cursor_pos = 0;

        int update_timer    = 0;
        int update_interval = 200;
        int nomber_in_cfg_list;
        const int buf_len = 7000;
        const int zero_buf_len = 50;

        //bool ready_to_delete = false;
        bool z_run = false;
        bool isa_file_ready = false;

        // глобальные переменные для фильтра слежения за нулем
        float teta1_1_1 = 0;
        float teta1_1_2 = 0;
        float u1_1 = 0;         // предыдущий результат фильтрации
        float zero_level = 0;       // значение нуля для проверки стабильности нуля
        float zero_level_old = -5000;   // значение нуля используемое в расчете массы
        float zero_level_old_old = -6000;

        point_ring_buf_ Datchik_buf = buf_len;
        ring_buf        zero_ring_buff;

        void count_sko();
        void sent_data_to_isa_thread();
        int  socket_init();

        void safe_pushback_data_buf_vector(const data_point &P);
        void update_adc_values_in_table();
        void start_train_obrab_thread();

        void write_data_to_temp_bin();
        void write_data_to_bin();

        void clear_train_data();

        bool dirExists(const std::string& dirName_in);

        void write_custom_bin_file();
        void write_custom_csv_file();
        //*************Фильтр, используемый для слежения за нулёи**************/

        //u - вход,
        //xx1_1:single;  //выход;
        //teta1_1_1, teta1_1_2, u1_1:single; //обновляемые переменные текужего состояния
        //pp1, pp2, kk0, kk1:single; //параметры фильтра, расчиваемые по постоянной времени fn (в тиках АЦР)

        //procedure flstep1_1(u:smallint);
        //begin
        //xx1_1 : = pp1 * teta1_1_1 + pp2 * teta1_1_2 + kk0 * u + kk1 * u1_1;
        //teta1_1_2: = teta1_1_1;
        //teta1_1_1: = xx1_1;
        //u1_1: = u
        //end;

        //Таблица значений параметров фильтра в зависимости от постоянной времени fn
        //fn     pp1                pp2                 kk0                    kk1
        //________________________________________________
        //100   1, 94398827719809 - 0, 945855730064412   0, 000942389222544467   0, 000925063643778076
        //150   1, 96273226169724 - 0, 963569991126732   0, 000421455417796542   0, 000416274011691925
        //50   1, 88737896185864 - 0, 894643062095681   0, 003699446415090560   0, 003564653821950450
        //200   1, 97207772859666 - 0, 972551145217778   0, 000237806349101347   0, 000235610272014663
        //300   1, 98140462342628 - 0, 981616010019566   0, 000106020154942667   0, 000105366438341595

        // возвращает -1 если нет наезда и № данных в пакете если наезд; обновление кольцевого буфера нулей
        // last_position - позиция послнднего ненулевого значения (съезда)
        int capture_zero(/*ring_buf* zeroes, беру из класса*/ unsigned char* buf, int& last_position);

        //*************!!Фильтр, используемый для слежения за нулёи!!**************/

        void copy_from_temp_to_bin(QString file_name);
        void write_csv(QFile* F, unsigned int val[], int nomb);  //Файл должен быть открыт заранее
        void write_csv(unsigned int val[], int nomb);
        void write_csv(data_point P);
        void write_isa(data_point d);
        //std::string get_filename_to_copy_isaev(std::string path, int weight_prog_num, int weight_num);
    public:
        Z_thread(DATA::BoardCfg *brd_cfg_pointer);
        Z_thread();
        ~Z_thread();
        void Z_thr_init(DATA::BoardCfg *brd_cfg_pointer);
        void run();
        void z_thread_init();
        void open_csv();
        void create_archiv_folder();
        void GetTempChanelData();
        void write_data_to_datchik_buf();  // заполнение циклического буфера

        void fill_graph_buff();
        void fill_membrane_graph_buff();

        int code_for_isaev_message(short ps1, short ps2);
    };


}   // namespace SGDK

#endif // Z_THREAD_H
