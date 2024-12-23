#ifndef RING_BUF_H
#define RING_BUF_H

#include <fstream>

struct data_point
{
    unsigned int time;
    uint16_t dat[4];

    int summ_all(){ return dat[0] + dat[1] + dat[2] + dat[3]; };
};

class point_ring_buf_
{
private:
    unsigned int len;
    union data_union
    {
        data_point P;
        unsigned char UC[6];
    };

    data_union* Dat;
    unsigned int idxIn;
    unsigned int idxOut;

public:
    unsigned int get_max_len();

    void set_point_data(data_point inDat);
    data_point get_point_data();

    unsigned int get_count();
    point_ring_buf_();					// Конструктор по умолчанию
    point_ring_buf_(unsigned int InitSize);
    void clear();
    ~point_ring_buf_();
};




class ring_buf
{
private:
    unsigned int len;
    union data_union
    {
        int I;
        float F;
        unsigned char UC[4];
    };

    data_union* Dat;
    unsigned int idxIn;
    unsigned int idxOut;

public:
    unsigned int get_max_len();
    void set_int_data(int inDat);
    void set_float_data(float inDat);
    int get_int_data();
    float get_float_data();
    unsigned int get_count();
    ring_buf();					// Конструктор по умолчанию
    ring_buf(unsigned int InitSize);
    void clear();
    ~ring_buf();
};




#endif // RING_BUF_H
