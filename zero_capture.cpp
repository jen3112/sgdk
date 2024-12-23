#include "zero_capture.h"



// возвращает -1 если нет наезда и № данных в пакете если наезд; обновление кольцевого буфера нулей
// last_position - позиция послнднего ненулевого значения (съезда)
int capture_zero(ring_buf* zeroes, unsigned char* buf, int& last_position)
{
    // коэфициенты фильтра
    float pp1 = 1.9439882771980;
    float pp2 = -0.94585573006441;
    float kk0 = 0.0009423892225444;
    float kk1 = 0.0009250636437780;

    int delta = 150;     // максимальный шум нуля
    int zero_level = 600;               // порог срабатывания наезда / съезда - потом буду читать из конфига

    float u;

    int rez = -1;

    // Проверка наезда
    if ((zero_level_old_old <= zero_level_old + delta) & (zero_level_old_old + delta >= zero_level_old))      // старый ноль стабилизировался
    {
        for (int i = 0; i < 16; i++)        // цикл по 16 точкам в пакете
        {
            u = (buf[7+i*8] + (buf[6+i*8] << 8)) + (buf[9+i*8] + (buf[8+i*8] << 8)) + (buf[11+i*8] + (buf[10+i*8] << 8)) + (buf[13+i*8] + (buf[12+i*8] << 8));

            if (u >= zero_level_old + zero_level)    // сработал наезд
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
        u = (buf[7] + (buf[6] << 8)) + (buf[9] + (buf[8] << 8)) + (buf[11] + (buf[10] << 8)) + (buf[13] + (buf[12] << 8));        // вход фильтра - сумма по 4 датчикам

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
        zeroes->set_float_data(xx1_1);

        //Для отладки
        int A = zeroes->get_count();
        int B = zeroes->get_max_len();

        // Если буфер заполнен, то извлекаю самое старое значение
        if (zeroes->get_count() == zeroes->get_max_len()-1)
        {
            zero_level_old_old = zero_level_old;
            zero_level_old = zeroes->get_float_data();   // самый дальний ноль
        }
    }
    return rez;
};
