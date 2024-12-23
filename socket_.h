#pragma once
#include "config.h"


int init_socket(Boards& brd_param)
{
    //...  инициализации Winsock  
    if (FAILED(WSAStartup(MAKEWORD(1, 1), &brd_param.ws)))
    {
        // Error...
        int error = WSAGetLastError();
        //...
        cerr << "SOCKET INIT ERROR";
    }

    // Заполним данными кроме адреса и порта
    ZeroMemory(&brd_param.saddr, sizeof(brd_param.saddr));
    // тип адреса (TCP/IP)
    brd_param.saddr.sin_family = AF_INET;


    //Заполняю адрес и порт из конфиг файла
    //адрес сервера. Т.к. TCP/IP представляет адреса в числовом виде, то для перевода
            // адреса используем функцию inet_pton.
    inet_pton(AF_INET, brd_param.IpStr.c_str(), &brd_param.saddr.sin_addr.s_addr);
    // Порт. Используем функцию htons для перевода номера порта из обычного в //TCP/IP представление.
    brd_param.saddr.sin_port = htons(brd_param.Port);


    
    return 0;
};

int  create_socket(Boards& brd_param)
{
    unsigned long block = 1;
    // создание сокета
    if (INVALID_SOCKET == (brd_param.soc = socket(AF_INET, SOCK_STREAM, 0)))
    {
        // Error...
        int error = WSAGetLastError();
        // ...
        cerr << "SOCKET CREATE ERROR";
    };

    //в неблокирующий режим 
    /*if (ioctlsocket((unsigned int)brd_param.soc, FIONBIO, &block) != 0)
    {
        printf("failed to set non-blocking socket\n");
        return false;
    };
    */

    if (SOCKET_ERROR == (connect(brd_param.soc, (sockaddr*)&brd_param.saddr, sizeof(brd_param.saddr))))
    {
        // Error...
        //error = WSAGetLastError();
        // ...
    }
    else
    {
        //соединение устанавливается
    }
    FD_ZERO(&brd_param.setR);
    FD_SET(brd_param.soc, &brd_param.setR);

    brd_param.time_out_val.tv_sec = 0;
    brd_param.time_out_val.tv_usec = 1000000;     // 1 сек. таймау

    select(brd_param.soc, &brd_param.setR, NULL, NULL, &brd_param.time_out_val);

    return 0;

}