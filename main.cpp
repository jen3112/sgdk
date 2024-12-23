#include "mainwindow.h"
#include "application.h"


int main(int argc, char *argv[])
{
    SGDK::application my_app(argc, argv);
    SGDK::MainWindow main_window;
    QString title = " СКЖД верия:  " +
                     QString::number(my_app.prog_version_number) + "." +
                     QString::number(my_app.prog_subvers_number) ;
    main_window.setWindowTitle( title );
    main_window.show();


    return my_app.exec();
}
