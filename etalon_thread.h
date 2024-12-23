#ifndef ETALON_THREAD_H
#define ETALON_THREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPort>


#define BOARD APP->pModel->Cfg.BrdConfVector[nomber_in_cfg_list]

//#define GR_WIN APP->gr_wins[nomber_in_cfg_list]

#define MEMBRANE_WIN APP->membrane_wins[nomber_in_cfg_list]

namespace SGDK
{
    class etalon_thread : public QThread
    {
    public:
        int com_error = 0;
        int etalon_thread_state=0;

        QSerialPort serialPort;

        unsigned char command[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB} ;
        char buff[8];

        int etalon_weigth = 0;

    public:
        etalon_thread();
        void run();
        void etalon_thread_init();

    };


}   // namespace SGDK

#endif //
