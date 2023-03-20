#include "mainmenu.h"
#include "onlineconfiguration.h"
#include "socketUtils.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainMenu w;
    w.show();

//    OnlineGame host(nullptr);
//    OnlineGame guest(nullptr);

//    host.setOnLineConfig("主机","",12345);
//    guest.setOnLineConfig("从机","127.0.0.1",12345);

//    host.show();
//    guest.show();

    return a.exec();
}
