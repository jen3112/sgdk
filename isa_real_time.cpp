#include <WinSock2.h>
#include <ws2tcpip.h>

#include "isa_real_time.h"
#include "application.h"

namespace SGDK
{

isa_real_time::isa_real_time()
{  }

void isa_real_time::clear_isa_buff()
{
    for(int i=0;i<204;i++)
    {
        buff[i]=0;
    }
}

/*---------------------------------------------------*/

int isa_real_time::isa_real_time_init()
{
    int error = 0;
    //...  инициализация Winsock
    if (FAILED(::WSAStartup(MAKEWORD(1, 1), &wsi)))
    {
        // Error...
        error = ::WSAGetLastError();
        //std::string S = "Thread " + std::to_string(BOARD.BrdNomb) + " WSA init Error";
        //SGDK::DATA::config::write_log(S);
        //Меняю статус платы
        APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status = false;
        ready_to_delete = true;
        return error;
    }

    // Заполним данными кроме адреса и порта
    ZeroMemory(&saddr, sizeof(saddr));

    // тип адреса (TCP/IP)
    saddr.sin_family = AF_INET; //стандартная инициализация
    saddr.sin_port = htons( 2000 );
    //преобразование QString Ip_str в char*
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    // создание сокета
    if (INVALID_SOCKET == (soc = ::socket(AF_INET, SOCK_STREAM, 0)))     // Канал А
    {
        // Error...
        error = WSAGetLastError();
        // ...
        //std::string Str = "Thread " + std::to_string(BOARD.BrdNomb) + " SOCKET create Error";
        //SGDK::DATA::config::write_log(Str);

        //Меняю статус
        APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status = false;
        ready_to_delete = true;
        return error;
    };

    /*int ret=*/ setsockopt(soc,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));  //установка таймаута сокета

    // выполняем соединение:
    if (SOCKET_ERROR == (::connect(soc, (sockaddr*)&saddr, sizeof(saddr))))
    {
        // Error...
        error = WSAGetLastError();
        //std::string S = "Thread " + std::to_string(BOARD.BrdNomb) + " connection Error";
        //SGDK::DATA::config::write_log(S);

        //Меняю статус платы
        APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status = false;
        ready_to_delete = true;
        return error;
    }

    return 0;
}

/*---------------------------------------------------*/

void isa_real_time::run()
{
    isa_thread_is_running = true;
    clear_isa_buff();

    if (!isa_real_time_init()) return; // инициализация соединения

    APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status = true;

    do  // основной цикл thread
    {
        if (buff_is_ready)
        {
            // пишу текущий пакет
            if (SOCKET_ERROR == (actual_len = send(soc, buff, len, 0)))
            {
                // Error...
                error = WSAGetLastError();
                //std::string S = "Thread " + std::to_string(BOARD.BrdNomb) + " reading socket Error";
                //SGDK::DATA::config::write_log(S);
            }
            else  // передача прошла успешно
            {  }
            buff_is_ready = false;
        }
        else
        {  }

    }while ( (!error)
             && (APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status)
             && !isa_force_exit);

    if (APP->model()->Cfg.BrdConfVector[board_id]->isa_connection_status)
    {
        // закрываю сокет
        closesocket(soc);
    }
    isa_thread_is_running = false;
}

}  // namespace SGDK
