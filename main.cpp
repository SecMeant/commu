#include <QApplication>
#include "initwindow.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    string nick;
    string ip;

    InitWindow wnd(&nick,&ip);
    wnd.show();

    app.exec();

    cout << "Nick: " << nick << '\n';
    cout << "IP: " << ip << '\n';
    return 0;
}
