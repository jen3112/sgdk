#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include "obrabotka_membrane.h"
#include "ring_buf.h"
#include "Matrix.h"
#include "data.h"
#include "application.h"
#include "wait.h"
namespace SGDK
{
    obrabotka_membrane::obrabotka_membrane() { };

/*-------------------------------------------------------------------*/

    void obrabotka_membrane::run()
    {
        ready_to_delete = false;

        OBR_BOARD->train_started = true;

        do
        {
            //надо добавить защиту от зависания, если слишком долго в цикле, то выход
            while( (data_buf_vector.size()>0)  && (!stop) ) // есть что обрабатывать
            {
                //получаю данные из буфера
                points_buf[points_buf_position] = data_buf_vector[0];
                erase_point_from_data_buf_vector();

                if (points_buf_position >= points_buf_len)
                {
                    // буфер заполнен, считаю производную методом МНК
                    // методом МНК - линейная аппроксимация
                    mnk_lin();

                    //заполняю поля под графиком
                    par->p_textblock->text()   = QString::number( max_x );
                    par->h_textblock->text()   = QString::number( max_y );
                    par->div_textblock->text() = QString::number( aprox_k );
                    par->prev_div_textblock->text()    = QString::number( prev_k );
                    par->divdiv_textblock->text()      = QString::number( div_k );
                    par->prev_divdiv_textblock->text() = QString::number( prev_div_k );

                    points_buf_position=0;

                    if ( (aprox_k + (div_k*len_x)) < 10)  // Предел достигнут
                    {
                        stop = true;

                        // Отключаю клапан подачи воздуха




                    }
                }
                else
                {
                    points_buf_position++;
                }
            }
        }
        while(!stop); // если буфер пуст, но максимум не достигнут

        stop = false;

        ready_to_delete = true;
    };

/*------------------------------------------------------------------------------------*/

    void obrabotka_membrane::mnk_lin()
    {
        long int a1 = 0;
        long int a2 = 0;
        long int mx = 0;
        long int my = 0;
        max_x = 0;
        min_x = 640000;
        max_y = 0;
        min_y = 640000;

        prev_k = aprox_k;
        prev_div_k = div_k;

        for (int i=0; i<points_buf_len; i++)
        {
            a1 += points_buf[i].d[0]*points_buf[i].d[1];
            a2 += points_buf[i].d[0]*points_buf[i].d[0];
            mx += points_buf[i].d[0];
            my += points_buf[i].d[1];
            if (points_buf[i].d[0] > max_x) max_x = points_buf[i].d[0];
            if (points_buf[i].d[0] < min_x) min_x = points_buf[i].d[0];
            if (points_buf[i].d[1] > max_y) max_y = points_buf[i].d[1];
            if (points_buf[i].d[1] < min_y) min_y = points_buf[i].d[1];
        }
        a1 = a1 / points_buf_len;
        a2 = a2 / points_buf_len;
        mx = mx / points_buf_len;
        my = my / points_buf_len;

        aprox_k = (a1-mx*my)/(a2-mx*mx);
        aprox_b = my - aprox_k*mx;
        len_x = max_x - min_x;
        div_k = (aprox_k - prev_k) / (max_x - min_x);

    }

/*------------------------------------------------------------------------------------*/

    void obrabotka_membrane::erase_point_from_data_buf_vector()
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

/*------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/

} // #namespace SGDK
