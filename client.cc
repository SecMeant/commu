// MESW CLIENT VERSION: 0.1 ( ONLY CONSOLE MODE )
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

#ifndef _MLP_CC
#define _MLP_CC
#include "mlp.cc"
#endif

// threadfunctions uses this define to import
// extern global vars
#ifndef _MESWCS_
#define _MESWCS_
#include "threadfunctions.h"
#endif

#pragma comment(lib, "ws2_32")

typedef unsigned int uint;

using std::cout;
using std::endl;
using std::cin;
using std::string;



// argv[1] = server ip address
#define serverip argv[1]
// argv[2] = clients nickname
#define nick argv[2]
int main(int argc, char** argv)
{
    if(argc != 3)
    {
        cout << "Usage: " << endl;
        cout << argv[0] << " <server's ipaddr> <nickname>" << endl;
        return 2;
    }

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
    cout << " OK" << endl;

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    SOCKET ConnectSocket;
    do
    {
        cout << "Filling address info . . .";
        iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
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
		char recvbuff[DEFAULT_BUFFLEN];

		char signal;
    string uid, data;
    mlProto mlp;
    mlp.fillFrame('0',"",argv[2]);
    sendbuff = mlp.packFrame();
    send(ConnectSocket, sendbuff.c_str(), sendbuff.size()+1 , 0);

	// forking recviving thread
	// sending address of ConnectSocket is ok because
	// only one connection is established on client side
	CreateThread(0,0,recvOverCS,(void*)&ConnectSocket,0,0);

	while(true)
	{
		string msg;
		cin >> msg;
		mlp.fillFrame('1',"",msg.c_str());
		sendbuff = mlp.packFrame();
		send(ConnectSocket, sendbuff.c_str(), sendbuff.size()+1,0);
	}

	cout << "Server closed connection, im out" << endl;

    return 0;
}
