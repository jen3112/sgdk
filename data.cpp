#include "data.h"
#include <QString>
#include <vector>
#include <fstream>
#include <QTime>
#include "application.h"
//#include "global_vars.h"

namespace SGDK
{
  namespace DATA
  {
      // trim string from both ends
      std::wstring trim(std::wstring s)
      {
          const wchar_t* typeOfWhitespaces = L" \t\n\r\f\v";
          s.erase(s.find_last_not_of(typeOfWhitespaces) + 1);
          s.erase(0, s.find_first_not_of(typeOfWhitespaces));
          return s;
      }






     void splitIp(std::wstring Str, int Out[])
      {
          std::wstring SubStr;

          for(short i=0; i<3; i++)
          {
              char pos = Str.find(L'.');
              if (pos>0) //найден второй символ :
              {
                  SubStr = trim(Str.substr(0, pos)); // подстрока до .
                  Str = trim(Str.substr(pos + 1));   // подстрока после .

                  Out[i] = stoi( SubStr );           // параметр раздела (номер платы)
              }
          }
          Out[3] = stoi( Str );          // последняя часть без точки в конце
      }


     bool config::semafor = false;


//  -------- class BoardCfg  ---------  //

     BoardCfg::BoardCfg()
     {
        z_thr.Z_thr_init(this);
     }

     // конструктор на основе мэпа конфигурации полученного при чтении файла конфига //
     BoardCfg::BoardCfg(const std::map<std::wstring,std::wstring> &conf_item_map)
     {
         subd_id  = -1; // Если не из базы данных, то -1

         if (conf_item_map.find(L"ip") != conf_item_map.end() )
         {
             // разбиваю сторку ip на фрагменты
             DATA::splitIp( conf_item_map.at(L"ip") , ip);
         }
         else
         {
             ip[0] = 192;
             ip[1] = 168;
             ip[2] = 0;
             ip[3] = 7;
         }

         ip_str = QString::number(ip[0]) + "."
                     + QString::number(ip[1]) + "."
                     + QString::number(ip[2]) + "."
                     + QString::number(ip[3])  ;

         if ( conf_item_map.find(L"port") != conf_item_map.end() )
         {
             port = QString::fromStdWString( conf_item_map.at(L"port") ).toInt();
         }

         if ( conf_item_map.find(L"write_csv") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"write_csv") == L"on" )
             {
                 write_csv = true;
             }
         }

         if ( conf_item_map.find(L"write_bin") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"write_bin") == L"on" )
             {
                 write_bin = true;
             }
         }

         if ( conf_item_map.find(L"write_zero") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"write_zero") == L"on" )
             {
                 write_zero = true;
             }
         }

         //write_zero_interval = 26;  // примерно раз в минуту
         if ( conf_item_map.find(L"write_zero_interval") != conf_item_map.end() )
         {
             write_zero_interval = QString::fromStdWString( conf_item_map.at(L"write_zero_interval") ).toInt();
         }

         if ( conf_item_map.find(L"process") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"process") == L"on" )
             {
                 process = true;
             }
         }

         if ( conf_item_map.find(L"mode") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"mode") == L"test" )
             {
                 BoardMode = DATA::BrdModes::test;
             }
             else if ( conf_item_map.at(L"mode") == L"stop" )
             {
                 BoardMode = DATA::BrdModes::stop;
             }
         }


         if ( conf_item_map.find(L"write_isa") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"write_isa") == L"on" )
             {
                 write_isa = true;
             }
         }

         if ( conf_item_map.find(L"isa_real_time") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"isa_real_time") == L"on" )
             {
                 isa_real_time = true;
             }
         }

         if ( conf_item_map.find(L"write_wheel") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"write_wheel") == L"on" )
             {
                 write_wheel = true;
             }
         }

         if ( conf_item_map.find(L"datchik_distance") != conf_item_map.end() )
         {
             datchik_distance = QString::fromStdWString( conf_item_map.at(L"datchik_distance") ).toInt();
         }

         if ( conf_item_map.find(L"power") != conf_item_map.end() )
         {
             power = QString::fromStdWString( conf_item_map.at(L"power") ).toInt();
         }

         if ( conf_item_map.find(L"kvant_to_weigth") != conf_item_map.end() )
         {
             kvant_to_weigth = QString::fromStdWString( conf_item_map.at(L"kvant_to_weigth") ).toInt();
         }
         //else
         //{
         //    kvant_to_weigth = SGDK::DATA::  Cfg.kvant_to_weigth_all;
         //}

         if ( conf_item_map.find(L"side") != conf_item_map.end() )
         {
             if(( conf_item_map.at(L"side") ) == L"L")
             {
                 side = 1;
             }
         }

         // Поиск статуса платы
         if ( conf_item_map.find(L"state") != conf_item_map.end() )
         {
             if ( conf_item_map.at(L"state") == L"on" )
             {
                 State = DATA::BoardState::On;
             }
         }

         if ( conf_item_map.find(L"isa_prog_dir") != conf_item_map.end() )
         {
             isa_prog_dir = QString::fromStdWString( conf_item_map.at(L"isa_prog_dir"));
         }

         if ( conf_item_map.find(L"isa_pause") != conf_item_map.end() )
         {
             isa_pause  = QString::fromStdWString( conf_item_map.at(L"isa_pause")).toInt();
         }

         if ( conf_item_map.find(L"isa_prg_nomb") != conf_item_map.end() )
         {
             isa_prg_nomb  = QString::fromStdWString( conf_item_map.at(L"isa_prg_nomb")).toInt();
         }

         if ( conf_item_map.find(L"max_timeout") != conf_item_map.end() )
         {
             max_timeout  = QString::fromStdWString( conf_item_map.at(L"max_timeout")).toInt();
         }

         show_graph[0] = true;
         show_graph[1] = true;
         show_graph[2] = true;
         show_graph[3] = true;
         show_graph[4] = false;
         show_graph[5] = false;

         z_thr.Z_thr_init(this);
     }

     // ------------------------------------------------- //

    // конструктор копирования
     BoardCfg::BoardCfg(const BoardCfg &bcfg)
     {
         subd_id  = -1; // Если не из базы данных, то -1

         ip[0] = bcfg.ip[0];
         ip[1] = bcfg.ip[1];
         ip[2] = bcfg.ip[2];
         ip[3] = bcfg.ip[3];

         ip_str = bcfg.ip_str;

         port = bcfg.port;

         write_csv = bcfg.write_csv;
         write_bin = bcfg.write_bin;
         write_zero = bcfg.write_zero;
         write_isa = bcfg.write_isa;
         write_zero_interval = bcfg.write_zero_interval;

         process = bcfg.process;
         BoardMode = bcfg.BoardMode;
         isa_real_time = bcfg.isa_real_time;
         write_wheel = bcfg.write_wheel;
         datchik_distance = bcfg.datchik_distance;
         power = bcfg.power;
         kvant_to_weigth = bcfg.kvant_to_weigth;

         side = bcfg.side;
         //State = DATA::Off;       // для проверки
         State = bcfg.State;
         isa_prog_dir = bcfg.isa_prog_dir;
         isa_pause = bcfg.isa_pause;
         isa_prg_nomb = bcfg.isa_prg_nomb;
         max_timeout = bcfg.max_timeout;
         write_csv = bcfg.write_csv;
         write_csv = bcfg.write_csv;

         show_graph[0] = true;
         show_graph[1] = true;
         show_graph[2] = true;
         show_graph[3] = true;
         show_graph[4] = false;
         show_graph[5] = false;

        z_thr.Z_thr_init(this);
     }
//  -------- end -- class BoardCfg  ---------  //


     config::config()  // конструктор
     {
        cfg_file_read();

     }


     void config::write_log_all(std::string S)
     {
         write_log_file(S);
         write_log_screen(S);
     }

     void config::write_log_screen(std::string S)
     {
         if(!APP->logTextEdit.isNull())
         {
             APP->logTextEdit->append(QString::fromStdString(S));   // - вылетает ошибка segmentation fault
         }
     }

     void config::write_log_file(std::string S)
     {
        bool ok ;
        int counter = 0;

        do
        {
            if (config::semafor == false)  // не занято
            {
                config::semafor = true; // блокирую запись в лог
                // Тут надо записать в конец лог файла и вывести в нижнюю панель
                std::ofstream LogFile ;
                LogFile.open("base.log", std::ios::app);
                S = QTime::currentTime().toString().toStdString() + " " + S;
                LogFile << S << std::endl;
                LogFile.close();

                config::semafor = false; // разблокировка
                ok=false;
            }
            else
            {
                ok=true;
                counter++; // жду окончания блокировки
            }
         } while ( ok & (counter<200) );
     }

     /* --------------------------------------------------- */

     void config::cfg_file_read()
     {
         std::wstring Str;
         std::wstring SubStr;
         int BrdNomb;

         enum cfg_blocks {none = 0, app_conf_block = 1, brd_conf_block = 2 };
         cfg_blocks current_block = none;

         //cfg_file_name().toStdString()
         std::string S = APP->cfg_file_name().toStdString();
         std::wifstream CfgFile (S);
         if (CfgFile.is_open())
         {
            // Пишу в лог что начался парсинг cfg файла
            //config::write_log("Start parsing cfg file.");

            int Count = 0;

            while ( getline (CfgFile,Str) )
            {
                Count++ ;
                // убираю лишние пробелы в начале и конце строки
                Str = DATA::trim(Str);

                if ( (Str[0] != '/') & (Str.size()>0) )       // проверка на строку комментария или пустую строку
                {
                    // проверка Str на начало блока (символ : в начале строки)
                    if(Str[0]==':')
                    {
                        // найдено начало нового блока
                        // убираю стартовый символ :
                        Str = DATA::trim(Str.substr(1));

                        //выделяю подстроку до след. символа: или до конца строки
                        int pos = Str.find(L":");

                        if (pos>0) //найден второй символ :
                        {
                            SubStr = DATA::trim(Str.substr(0, pos)); // подстрока до :
                            Str = DATA::trim(Str.substr(pos + 1)); // подстрока после :
                            BrdNomb = stoi( Str );     // параметр раздела (номер платы)
                        }
                        else
                        {
                            SubStr = Str;
                            Str.clear();
                            BrdNomb = -1;   // все платы
                        };

                        // проверка имени раздела
                        if(!SubStr.compare(L"app"))
                        {
                            current_block = app_conf_block;
                        }
                        else if(!SubStr.compare(L"brd"))
                        {
                            current_block = brd_conf_block;
                        }
                        else
                        {
                            current_block = none;
                        }

                    }  // конец проверки Str на начало блока
                    else if (current_block != none) // блок уже выбран
                    {   // считанная строка не является началом блока

                        //выделяю подстроку до след. символа: или до конца строки
                        int pos = Str.find(L":");

                        // разбиваю строку на параметр и значение
                        SubStr = DATA::trim(Str.substr(0, pos)); // подстрока до :
                        Str = DATA::trim(Str.substr(pos + 1)); // подстрока после :

                        // запись в map
                        switch(current_block)
                        {
                        case app_conf_block:
                            AppCfg[SubStr] = Str;
                            break;
                        case brd_conf_block:
                            BrdCfg[BrdNomb][SubStr] = Str;
                            break;
                        default:
                            break;
                        }
                    }
                }  // конец проверки пустой строки или комментария
            }

            CfgFile.close();
         }
         else
         {
             // пишу в лог что нет cfg файла
             config::write_log_all("Cfg file not found \n");
         }
     }

     config::~config() // удаление всех элементов вектора конфигов плат
     {
         BrdConfVector.clear();
     }
  }
}   // namespace SGDK
