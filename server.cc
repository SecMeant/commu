// This macro prevets windows.h from including winsock.h 1.1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// ws2tcpip.h uses this constant to import
// structs: getaddrinfo and freeaddrinfo
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include "user.cc"
#include "mlp.cc"

#pragma comment(lib,"ws2_32.dll")

#define DEFAULT_PORT "31337"
#define DEFAULT_BUFFLEN 1024

unsigned int User::counter;

using std::cout;
using std::endl;
using std::cin;
using std::vector;
using std::string;

// global vector of users connected to the server
vector<User> users;

int main(void)
{
    WSADATA wsaData;
    int iResult;

    cout << "Checking availability of WS2_32.dll . . .";
    // version 2.2
    // setting up winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    
    // checks if WS2_32.dll is imported
    if(iResult != 0)
    {
        // iResult contains returned error id
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }
    cout << " OK" << endl;

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    cout << "Filling address info . . .";
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0)
    {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup(); // frees Winsock 2 DLL
        return 1;
    }
    cout << " OK" << endl;

    cout << "Creating socket . . .";
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(ListenSocket == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup(); // frees Winsock 2 DLL
        return 1;
    }
    cout << " OK" << endl;

    cout << "Binding connection . . .";
    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {
        cout << "Bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    cout << " OK" << endl;

    freeaddrinfo(result);

    char signal;
    string uid, data;
    mlProto mlp;

    while(true)
    {
        cout << "Preparing for listening . . .";
        if( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR )
        {
            cout << "Listen failed with error: " << WSAGetLastError() << endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        cout << " OK - Socket listening." << endl;

        SOCKET ClientSocket;
        
        cout << "Preparing to accept clinet's connection . . .";
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if(ClientSocket == INVALID_SOCKET)
        {
            cout << "Accept failed: " << WSAGetLastError() << endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        cout << " New user connected.";
        char recvbuf[DEFAULT_BUFFLEN];

        do
        {
            iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFFLEN, 0);
        }while (iResult > 0);

        mlp.unpackFrame(recvbuf);

        mlp.getFrame(signal, uid, data);

        cout << " Nickname: " << data << endl;

        if(signal == '0')
        {
            User u1(ClientSocket, data);
            users.push_back(u1);
        }

        /*
        cout << "\n\n\nUSERS: " << endl;
        for(int i=0; i < users.size(); i++)
            users[i].show();
        */
    }

    return 0;
}


