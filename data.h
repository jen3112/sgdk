#ifndef DATA_H_
#define DATA_H_

#include <QString>
#include <QFile>
#include <vector>
#include <map>

#include <WinSock2.h>

#include "graph.h"
#include "int_ring_buf.h"
#include "z_thread.h"

namespace SGDK
{
 class graph;

namespace DATA
{
    // общие для всего приложения данные

    // Возможно, сюда будет лучше словарь добавить
    enum BoardState : unsigned char
    {
        Off             = 0,
        On              = 1,
        Pingable        = 2,
        Connecting      = 3,
        Connected       = 4,
        Train_detection = 5,
        Disconnected    = 6
    };

    enum BrdModes : unsigned char
    {
        test    = 0,
        nonstop = 1,
        stop    = 2
    };

    enum Temperature_units : unsigned char
    {
        grad = 0,
        kvant = 1
    };

    class BoardCfg   // Структура для хранения настроек платы
    {
    public:
        SGDK::graph gr_win;
        SGDK::Z_thread z_thr;

        int         subd_id  = 0;  // id в базе данных
        int         BrdNomb = 0;
        int         row_nomb_in_table = 0;

        int         ip[4] = {192,168,0,0};
        QString     ip_str;
        int         port = 23;
        BoardState  State   = DATA::BoardState::Off;
        BrdModes    BoardMode = DATA::BrdModes::nonstop;  // по умолчанию;
        int         timeout_counter = 0;
        int         max_timeout = 5;   //Через 5 срабатываний таймера будет попытка переподключения
        int         isa_pause = 800;
        int         isa_prg_nomb = 2;
        int         FREQ = 0;
        int         BrdTime = 0;
        int         BrdDacType = -1;
        int         BrdTime_high = 0;
        int         PowerManagementType = 0;
        int         ch_nomb = 4;
        bool        ch_nomb_changed = 0;

        int z_thread_status=0;

        int temperature_limit = 0;
        int power_mode = 0;

        bool  Chanel_group_list_active = false;

        QString     BrdVersion = "0.0";

        unsigned char buff[136];

        bool train_started = false;
        bool train_stopped = false;
        bool isa_connection_status = false;
        bool process = false;
        bool isa_real_time = false;
        bool write_isa = false;
        bool write_csv = false;
        bool write_bin = false;
        bool write_wheel = false;
        bool write_zero = false;
        int  write_zero_interval = 300; // в секундах (*125 = в пакетах)

        bool show_graph[18] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
        bool metrol_enabled;

        int metrol_zero_data[4] = {0};
        int metrol_data[16]    = {0};       // 4 точки
        int metrol_zero[4]     = {0};

        // Сокет сетевого соединения
        SOCKET s;

        // для подсчета СКО
#define SCO_BUFF_LEN 800
        int sko_buff_poz = 0;
        int sko_buff[4][SCO_BUFF_LEN] = {{0}};  // 400 точек = 25 пакетов по 16 точек
        int sko[4]  =  {0};
        int delta[4] = {0};  // разница между min и max

        // Данные для передачи в окна графиков
        uint16_t max_buf_len = 6000;

        uint32_t cur_time = 0;
        int start_membrane_graph = 0;
        int membrane_mode_started = 0;

        unsigned short Temp_kvanty[4] = {0};
        unsigned short Temp_grad[4]   = {0};

        QString csv_file_name;
        QString isa_file_name;
        QString bin_file_name;

        QString isa_prog_dir;

        unsigned int DAC[16] = {0};
        unsigned int ADC[16] = {0};

        double kvant_to_weigth = 0.001;

        // индивидуальные коэффициенты датчиков
        float K1;
        float K2;
        float K3;
        float K4;

        // расстояния между датчиками в мм
        int datchik_distance = 250;
        int power  = 20;   // степень функции апроксимации
        int side = 0;

        bool write_custom_bin = false;
        QFile  *custom_bin_file;
        QString custom_bin_file_name;

        bool write_custom_csv = false;
        QFile  *custom_csv_file;
        QString custom_csv_file_name;

        virtual ~BoardCfg(){};  // Нужна для работы dynamic_cast

        BoardCfg(const BoardCfg &bcfg);   // конструктор копирования
        BoardCfg();

        // конструктор объекта конфига на основе map-записи
        BoardCfg(const std::map<std::wstring,std::wstring> &conf_item_map);
    };

    class config
    {
      public:
        std::vector<BoardCfg*> BrdConfVector; // конфиги всех плат
        QString archiv_dir; // путь куда пишутся все проезды
        int max_arhive_nomb = 3200;
        //int arhive_counter = 0;

        int update_timer_interval_normal = 600;
        int update_timer_interval_max = 1600;

        // размеры окон

        int main_win_size_x = 1600;
        int main_win_size_y = 500;



        int etalon_com_port_name;

        Temperature_units temperature_mode = grad;

        bool archive_folder_created = false;
        QString archive_folder;

        // Допустимый диапазон ЦАПов
        unsigned int Min_Dac_value = 500;
        unsigned int Max_Dac_value = 2300;
        unsigned int Recomended_Min_Dac_value = 1000;
        unsigned int Recomended_Max_Dac_value = 2000;


        // сырые данные из kонфиг файла
        std::map<std::wstring,std::wstring> AppCfg;       // Данные настройки программы
        //std::map<std::string,std::string> AllBrdCfg;    // Общие данные всех плат (коэфф пересчета веса, ...)
        std::map<int,std::map<std::wstring,std::wstring>> BrdCfg;  // данные плат по отдельность int- № платы

        std::wstring trim(std::wstring s);

        //общая конфигурация всей системы
        double kvant_to_weigth_all = 0.000378; // если не задан на плату, то использую этот

        void cfg_file_read();    //читает сырые данные из конфиг файла

        static bool semafor;

        static void write_log_file(std::string S);
        static void write_log_all(std::string S);
        static void write_log_screen(std::string S);

        config();
        ~config();  // удаление всех элементов вектора конфигов плат
    };

    void splitIp(std::wstring Str, int Out[]);

}   // namespace DATA

}  // namespace SGDK

#endif // DATA_H
