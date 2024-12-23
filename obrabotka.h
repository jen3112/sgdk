#ifndef OBRABOTKA_H
#define OBRABOTKA_H

#include <vector>
#include <QThread>
#include <QFile>
#include <fstream>

#include "ring_buf.h"
#include "Matrix.h"
//#include "data.h"

//#define OBR_BOARD APP->pModel->Cfg.BrdConfVector[board_id]

namespace SGDK
{
namespace DATA
{
    class BoardCfg;
}



    class obrabotka : public QThread
    {
      public:
        DATA::BoardCfg* board_obrab_cfg_pointer;

        struct wheel_calculation  // все расчеты по колесам
        {
            bool direction = true;
            bool stopped_wheel = false;
            bool square_aproxim_error = false;

            int wheel_nomb = 0;
            int van_nomb   = 0;   // номер вагона когда определился
            int van_type = 0 ;    // >0 вагоны, <0 - локомотивы, 0 - не определен

            int zero_level = 0;
            int osc_size;
            int osc_max_x;
            int osc_max_y;
            int trim_dolja;
            int trim_level;
            int trim_l_poz;
            int trim_r_poz;
            int trimmed_osc_size;
            // положение центра параболы при первой апроксимации
            double square_aproxim_xmax = 0;
            double square_aproxim_ymax = 0;
            double Koeffic[3];

            // уточненное положение центра параболы
            double xmax = 0;
            double ymax = 0;
            double b = 0;
            double a = 0;
            double weigth = 0;

            int wheel_time = 0;    // колесо между датчиками
            int wheel_start_time = 0;
            int wheel_end_time = 0;

            double speed0 = 0;  //скорость по ширине проезда
            double speed_corr = 0; // средняя скорость с учетом пред и послед осей
            double prev_distance = 0; // расстояние до пред. оси в пределах вагона
            double corr_prev_distance = 0; // после корректировки скорости
        };

        data_point stop_test_buf[20];
        std::vector <wheel_calculation> wh_calc;
        std::vector <data_point> data_buf_vector;
        std::vector<data_point> wheel_data;
        std::vector<data_point> corrected_wheel_data;

        bool wh_stopped = false;
        bool local_stop_flag = false;
        bool ready_to_delete = false;
        bool capture_wheel_started;
        bool endwheel;

        bool data_buf_vector_ready_semafor = true;
        bool stop = false;

        int test_stop_counter = 0;
        int prev_vag_last_wheel_nomb = -1; // № последнего колеса пред вагона
        int van_nomb = 0;
        int cart_len=0;
        int cart_wheel_nomb=0;
        int cart_wheel_count_stop = 0;
        int cart_distance = 0;
        int default_wheel_wait_timer = 50;
        int wheel_wait_timer_max     = default_wheel_wait_timer ;
        int end_wheel_timer = 15; // число точек после съезда для защиты от дребезга
        //int board_nomb_in_cfg_list;
        int wheel_nomb=0;
        int zero_level;         // уровень 0 перед началом проезда
        int delta_level = 800;   // порог срабатывания наезда / съезда - потом буду читать из конфига
        int prev_wheel_time;

        const int len = 30;
        point_ring_buf_ pre_wheel_buf =  len; //небольшой кольцевой буфер для хранения начального фрагмента колеса

        double prev_speed_0;
        double Koeffic[3];
        double xmax, ymax;
        double diap_coeff;
        double a = 0;
        double b = 0;

        obrabotka();
        ~obrabotka();
        void run();            // запуск потока обработки проезда

        int find_zero_point(int &interval_begin, int &interval_end, int sum_number);
        void clear_stop_test_buf();
        void start_capture();
        void detect_wheel_end(int sum_level);
        void erase_point_from_data_buf_vector();
        void create_wheel_folder();

        void calculate_wheel();
        void calculate_stopped_wheel();
        void calculate_nonstop_wheel();
        double calc_speed_isaev();
        double calc_speed_zero_method();
        void segmentate_andreich();
        void segmentate_simple();

        bool test_stop(data_point point_dat);

        void clear_wheel_data();
        void write_wheel_to_csv(QString FileNameSuffix);
        void aproximate(double &a,double &b);

        void trim_wheel_data();
        void square_aproxim(double &xmax,double &ymax);
        void wheel_to_screen(int w_n);
        void write_aprox_csv();
        void save_wheels_log();

        int calc_dolja_to_trim_wheel();

        double power_function(double X);
    };
} //#namespace SGDK

#endif // OBRABOTKA_H
