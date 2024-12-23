#ifndef ZAHVAT_H
#define ZAHVAT_H

#include <QObject>
#include <QDataStream>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")

namespace SGDK
{
    class Zahvat : public QObject
    {
        Q_OBJECT
    public:
        explicit Zahvat(QObject *parent = 0);
        //virtual ~Zahvat();

        char BrdNomb;
        unsigned char SocketBuf[135]; // Данные полученные из сокета

 //       QTcpSocket *socket;

    signals:

    public slots:
    };

} // namespace SGDK
#endif // ZAHVAT_H
