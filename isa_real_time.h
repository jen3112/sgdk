#ifndef ISA_REAL_TIME_H
#define ISA_REAL_TIME_H

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <QThread>

namespace SGDK
{

class isa_real_time : public QThread
{
public:
    WSADATA wsi;
    int error = 0;
    sockaddr_in saddr;
    SOCKET soc;
    int actual_len;
    int len = 204;
    char buff[205];

    int isa_real_time_init();

    bool isa_thread_is_running = false;
    bool isa_force_exit = false;
    void clear_isa_buff();

    bool buff_is_ready = false;
    int board_id;

    bool ready_to_delete = false;

    int timeout = 1000;

    isa_real_time();
    void run();
};

}  // namespace SGDK


#endif // ISA_REAL_TIME_H
