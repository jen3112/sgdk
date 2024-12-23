#ifndef OBRABOTKA_MEMBRANE_H
#define OBRABOTKA_MEMBRANE_H

#include <vector>
#include <QThread>
#include <QFile>
#include <fstream>

//#include "test_membrane.h"
#include "ring_buf.h"
#include "Matrix.h"

#define OBR_BOARD APP->pModel->Cfg.BrdConfVector[board_id]

//class SGDK::test_membrane;

namespace SGDK
{
    class test_membrane;
    class obrabotka_membrane : public QThread
    {
      public:
        struct point
        {
            unsigned int d[2];
        };



        static const int points_buf_len = 300;
        int points_buf_position = 0;
        point points_buf[points_buf_len];

        double aprox_k = 0;
        double aprox_b = 0;

        double prev_k = 0;
        //double prev_prev_k = 0;
        double div_k = 0;
        double prev_div_k = 0;
        unsigned int len_x = 0;
        unsigned int max_x = 0;
        unsigned int min_x = 640000;
        unsigned int max_y = 0;
        unsigned int min_y = 640000;

        void mnk_lin();

        std::vector <point> data_buf_vector;

        bool local_stop_flag = false;
        bool ready_to_delete = false;
        bool capture_wheel_started;
        bool endwheel;
        bool write_wheel = false;
        bool data_buf_vector_ready_semafor = true;
        bool stop = false;

        int test_stop_counter = 0;

        int board_id;

        const int len = 30;

        double Koeffic[3];
        double xmax, ymax;
        double diap_coeff;
        double a = 0;
        double b = 0;

        test_membrane* par;  //сюда надо при инициализации передать указ-ль на родителя

        obrabotka_membrane();
        virtual ~obrabotka_membrane() {};
        void run();            // запуск потока обработки проезда

        void start_capture();
        void erase_point_from_data_buf_vector();

    };
} //#namespace SGDK

#endif // OBRABOTKA_H
