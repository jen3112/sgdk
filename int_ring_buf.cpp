#include "int_ring_buf.h"
//#include <QString>

//#include <vector>
//#include <map>


long int int_ring_buf::get_max_len()
{
    return len;
}

void int_ring_buf::set_data(long int inDat)
{
    Dat[idxIn] = inDat;
    idxIn++;
    if (idxIn >= len) idxIn = 0;
    if (idxIn == idxOut) idxOut++;
    if (idxOut >= len) idxOut = 0;
};


long int int_ring_buf::get_data()
{
    long int D;
    D = Dat[idxOut];

    idxOut++;
    if (idxOut >= len) idxOut = 0;
    return D;
};

long int int_ring_buf::get_count()
{
    long int retval = 0;
    if (idxIn < idxOut) retval = len + idxIn - idxOut;
    else retval = idxIn - idxOut;
    return retval;
}
int_ring_buf::int_ring_buf()  // Конструктор по умолчанию
{
    len = 4000;
    Dat = new long int[len];
    clear();
};

int_ring_buf::int_ring_buf(long int InitSize)  // Конструктор
{
    len = InitSize;
    Dat = new long int[len];
    clear();
};

int_ring_buf::~int_ring_buf()  // Деструктор по умолчанию
{
    delete[] Dat;
};

void int_ring_buf::clear()
{
    idxIn = 0;
    idxOut = 0;
};


