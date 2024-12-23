



// base -  эффексивное расстояние межу датчиками. От него зависит форма апроксимирующей функции (20, 25,30,35,40,45)
// zero_level - уровень нуля перед наездом
float count_weigth(std::vector <unsigned int> weel_data, int zero_level, int base, float &a, float &b)
{
    //float time_coef = (float)template_func.size() / weel_data.size() ;

    float sum1 = 0;
    float sum2 = 0;

    int interval = weel_data.size() ;
    int simmetrich_interval = interval / 2;

    float sum_xy = 0;

    float sum_xx = 0;
    float sum_x = 0;
    float sum_y = 0;

    for (int t = 0; t < interval; t++)
    {
        float y = weel_data[t] - zero_level;
        float x = aprox_func(t - simmetrich_interval, base);

        sum_xy += x * y;
        sum_xx += x * x;
        sum_x += x;
        sum_y += y;
    }

    a = (interval * sum_xy - sum_x * sum_y) / (interval * sum_xx - sum_x * sum_x);
    b = (sum_y - a * sum_x) / interval;

    float sko = 0;

    for (int t = 0; t < interval; t++)
    {
        float y = weel_data[t] - zero_level;
        float x = aprox_func(t - simmetrich_interval, base)*a + b;

        sko += (x - y) * (x - y);
    }
    sko = sqrt(sko / interval);


    return sko;
};

