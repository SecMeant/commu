#ifndef CHATWINDOW_H
#define CHATWINDOW_H

// This macro prevents windows.h from including winsock 1.1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// ws2tcpip.h uses this constant to import
// structs: getaddrinfo and freeaddrinfo
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#define DEFAULT_PORT "31337"
#define DEFAULT_BUFFLEN 1024

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include <string>
#include <QString>

#include "mlp.h"

// threadfunctions uses this define to import
// extern global vars
#ifndef _MESWCS_
#define _MESWCS_
#include "threadfunctions.h"
#endif

// qt libs
#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QScrollArea>
#include <QScrollBar>
#include <QKeyEvent>
#include <QtCore> // Qt::AlignRight

using namespace std;

class ChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWindow(SOCKET &cs,QWidget* parent = 0);
    ChatWindow& operator<<(const string msg);
    ChatWindow& operator<<(const char* msg);
    SOCKET getSock();

private:
    QPushButton* send_button;
    QScrollArea* chat_main;
    QLabel* chat_label;
    QLineEdit* msg_text;

    SOCKET connectSocket;

    // is runned in separate receiving thread
    // appending data from socket to window label
    friend DWORD WINAPI recvOverCS(void *);

signals:
    // used to invoke slot from child thread
    // what is illegal with direct call from child thread
    void adjustLabelSignal();

private slots:
    void sendButtonClicked();
    void adjustLabelSlot();
    void sendMSG();
};


#endif // CHATWINDOW_H
