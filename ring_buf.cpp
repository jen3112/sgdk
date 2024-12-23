#include "ring_buf.h"


unsigned int ring_buf::get_max_len()
{
    return len;
}

void ring_buf::set_int_data(int inDat)
{
    Dat[idxIn].I = inDat;
    idxIn++;
    if (idxIn >= len) idxIn = 0;
    if (idxIn == idxOut) idxOut++;
    if (idxOut >= len) idxOut = 0;
};



void ring_buf::set_float_data(float inDat)
{
    Dat[idxIn].F = inDat;
    idxIn++;
    if (idxIn >= len) idxIn = 0;

    if (idxIn == idxOut) idxOut++;
    if (idxOut >= len) idxOut = 0;
};



int ring_buf::get_int_data()
{
    int D;
    D = Dat[idxOut].I;

    idxOut++;
    if (idxOut >= len) idxOut = 0;
    return D;
};



float ring_buf::get_float_data()
{
    float D;
    D = Dat[idxOut].F;

    idxOut++;
    if (idxOut >= len) idxOut = 0;
    return D;
};



unsigned int ring_buf::get_count()
{
    unsigned int retval = 0;
    if (idxIn < idxOut) retval = len + idxIn - idxOut;
    else retval = idxIn - idxOut;
    return retval;
}



ring_buf::ring_buf()  // Конструктор по умолчанию
{
    len = 6000;
    Dat = new data_union[len];
    clear();
};



ring_buf::ring_buf(unsigned int InitSize)  // Конструктор
{
    len = InitSize;
    Dat = new data_union[len];
    clear();
};



ring_buf::~ring_buf()  // Деструктор по умолчанию
{
    delete[] Dat;
};



void ring_buf::clear()
{
    idxIn = 0;
    idxOut = 0;
};

unsigned int point_ring_buf_::get_max_len()
{
    return len;
}



void point_ring_buf_::set_point_data(data_point inDat)
{
    Dat[idxIn].P = inDat;
    idxIn++;
    if (idxIn >= len) idxIn = 0;

    if (idxIn == idxOut) idxOut++;
    if (idxOut >= len) idxOut = 0;
};


data_point point_ring_buf_::get_point_data()
{
    data_point D;
    D = Dat[idxOut].P;

    idxOut++;
    if (idxOut >= len) idxOut = 0;
    return D;
};


unsigned int point_ring_buf_::get_count()
{
    unsigned int retval = 0;
    if (idxIn < idxOut) retval = len + idxIn - idxOut;
    else retval = idxIn - idxOut;
    return retval;
}
point_ring_buf_::point_ring_buf_()  // Конструктор по умолчанию
{
    len = 6000;
    Dat = new data_union[len];
    clear();
};

point_ring_buf_::point_ring_buf_(unsigned int InitSize)  // Конструктор
{
    len = InitSize;
    Dat = new data_union[len];
    clear();
};

point_ring_buf_::~point_ring_buf_()  // Деструктор по умолчанию
{
    delete[] Dat;
};

void point_ring_buf_::clear()
{
    idxIn = 0;
    idxOut = 0;
};



