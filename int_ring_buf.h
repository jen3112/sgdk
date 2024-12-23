#ifndef INT_RING_BUF_H
#define INT_RING_BUF_H

//#include <QString>

//#include <vector>
//#include <map>

class int_ring_buf
{
private:
    long int len;

    long int* Dat;

    //data_union* Dat;

    long int idxIn;
    long int idxOut;

public:
    long int get_max_len();
    void set_data(long int inDat);

    long int get_data();

    long int get_count();
    int_ring_buf();					// Конструктор по умолчанию
    int_ring_buf(long int InitSize);
    void clear();
    ~int_ring_buf();
};




#endif // RING_BUF_H
