// qt libs
#include <QApplication>
#include "initwindow.h"
#include "chatwindow.h"

typedef unsigned int uint;

using std::cout;
using std::endl;
using std::cin;
using std::string;

int main(int argc, char** argv)
{
    // qt event loop
    QApplication app(argc, argv);

    // vars to be passed to init window
    // it will fill them with nickname and serverip
    string nick;
    string ip;
    bool ret;

    InitWindow wnd(&nick,&ip,&ret);
    wnd.show();

    app.exec();

    if(ret == 0)
        return 1;

    WSADATA wsaData;
    int iResult;

    cout << "Checking availability of WS2_32.dll . . .";
    // version 2.2
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    // checks if ws2_32.dll is imported
    if(iResult != 0)
    {
        // iResult contains returned error id
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }
    cout << " OK" << endl << endl;

    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    SOCKET ConnectSocket;
    do
    {
        cout << "Filling address info . . .";
        iResult = getaddrinfo(ip.c_str(), DEFAULT_PORT, &hints, &result);
        if(iResult != 0)
        {
            cout << "getaddrinfo failed: " << iResult << endl;
            WSACleanup();
            return 1;
        }
        cout << " OK" << endl;

        ConnectSocket = INVALID_SOCKET;

        cout << "Filling socket info . . .";
        ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

        if(ConnectSocket == INVALID_SOCKET)
        {
            cout << "Error at socket(): " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }
        cout << " OK" << endl;

        cout << "Connecting to the server. . .";
        iResult = connect( ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
        if(iResult == SOCKET_ERROR)
        {
            cout << "Error at connect(): " << WSAGetLastError() << endl;
            cout << "Retry in 5secs . . ." << endl;
            Sleep(5000);
        }

    }while(iResult == SOCKET_ERROR);
    cout << " OK" << endl;

    freeaddrinfo(result);

    if(ConnectSocket == INVALID_SOCKET)
    {
        cout << "Unable to connect to server" << endl;
        WSACleanup();
        return 1;
    }
    else
        cout << "Connected to the server" << endl;


    string sendbuff;
    mlProto mlp;
    mlp.fillFrame('0',"",nick.c_str());
    sendbuff = mlp.packFrame();
    send(ConnectSocket, sendbuff.c_str(), sendbuff.size()+1 , 0);

    ChatWindow w(ConnectSocket);
    w.show();

    // forking recviving thread
    // sending address of ConnectSocket is ok because
    // only one connection is established on client side
    CreateThread(0,0,recvOverCS,(void*)&w,0,0);

    app.exec();

    return 0;
}
