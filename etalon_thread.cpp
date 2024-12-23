#include <QString>
#include <QtSerialPort/QSerialPort>

#include "etalon_thread.h"
#include <windows.h>
#include "application.h"

namespace SGDK
{
    etalon_thread::etalon_thread()  {  }

    /*------------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------------*/

    void etalon_thread::etalon_thread_init()
    {
       com_error = 0;
       etalon_thread_state = 0;

       // указали имя к какому порту будем подключаться

       //QString S = APP->pModel->Cfg.etalon_com_port_name;
       //serialPort.setPortName("com7");
       QString com_port = "com"+ QString::number(APP->pModel->Cfg.etalon_com_port_name);
       serialPort.setPortName(com_port);
       // указали скорость
       serialPort.setBaudRate(QSerialPort::Baud9600);
       serialPort.setDataBits(QSerialPort::Data8);
       serialPort.setStopBits(QSerialPort::OneStop);
       serialPort.setParity(QSerialPort::NoParity);
       serialPort.setFlowControl(QSerialPort::NoFlowControl);

       // пробуем подключится
       if (!serialPort.open(QIODevice::ReadWrite)) {
           // если подключится не получится, то покажем сообщение с ошибкой
           com_error = 1;
           return;
       }
       etalon_thread_state = 1;  // подключено
    }

    /*------------------------------------------------------------------------------*/

    void etalon_thread::run()  // переопределение метода который будет запущен в новом потоке
    {
        etalon_thread_init();
        QByteArray readData;

        if (com_error == 0)
        {
            do  // основной цикл thread
            {
                serialPort.write((const char*)command,8);
                serialPort.waitForBytesWritten();

                readData.clear();
                readData = serialPort.readAll();

                Sleep(20);

                char buf[2];

                while (serialPort.waitForReadyRead(50))
                {
                    int count = 0;
                    do
                    {
                        serialPort.read(buf,1);
                        count++;
                    }while (count<10 && buf[0]!=1);

                    serialPort.read(buf,1);

                    readData.append(serialPort.readAll());
                    if (readData.length()<7)
                    {
                        //Sleep(20);
                        while (serialPort.waitForReadyRead(50))
                        {
                            // если не все пришло то дочитываю хвост
                            readData.append(serialPort.readAll());
                            buf[1]=0;
                        }
                    }

                    //count++;
                }

                if (serialPort.error() == QSerialPort::ReadError)
                {
                    //standardOutput << QObject::tr("Failed to read from port %1, error: %2")
                    //                  .arg(serialPort.errorString())
                    //               << "\n";
                    etalon_thread_state = 0;
                    com_error = 1;
                }
                else if (serialPort.error() == QSerialPort::TimeoutError && readData.isEmpty())
                {
                    //etalon_thread_state = 0;
                    //com_error = 1;
                }

                // данные получены, вычисляю вес
                if ( (readData.length()==7) && (readData[0]==4))
                {
                    unsigned int w =((unsigned char)readData[1]<<24) +
                                    ((unsigned char)readData[2]<<16) +
                                    ((unsigned char)readData[3]<<8) +
                                    (unsigned char)readData[4];

                    etalon_weigth = (int)w*10;

                }
                else
                {
                    //etalon_weigth = -2;
                }



            }while( (!com_error) && (etalon_thread_state) );      //Если ошибка чтения то выхожу

        }

        readData.clear();
        if (serialPort.isOpen()) serialPort.close();
        etalon_thread_state = 0;  // отключение

    }

    /* --------------------------------------------------- */


}   //namespace SGDK
