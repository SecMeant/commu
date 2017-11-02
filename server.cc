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
#include <list>
#include <string>
#include "user.cc"

// threadfunctions uses this define to import
// extern global var
#ifndef _MESWSS_
#define _MESWSS_
#include "threadfunctions.h"
#endif

#ifndef _MLP_CC
#define _MLP_CC
#include "mlp.cc"
#endif

#pragma comment(lib,"ws2_32")

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "31337"
#endif

#ifndef DEFAULT_BUFFLEN
#define DEFAULT_BUFFLEN 1024
#endif

unsigned int User::counter;

using std::cout;
using std::endl;
using std::cin;
using std::list;
using std::string;

// global vector of users connected to the server
list<User> users;

long unsigned int recvOver(void * ud);
	
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
	
mainLoop:
	while(true)
	{
			cout << "Preparing for listening . . .";
			if( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR )
			{
				cout << "Listen failed with error: " << WSAGetLastError() << endl;
				goto mainLoop;
			}
			cout << " OK - Socket listening." << endl;

			SOCKET ClientSocket;
			
			cout << "Preparing to accept clinet's connection . . .";
			ClientSocket = accept(ListenSocket, NULL, NULL);
			if(ClientSocket == INVALID_SOCKET)
			{
				cout << "Accept failed: " << WSAGetLastError() << endl;
				goto mainLoop;
			}
			cout << "\nNew user connected.";

			string sendbuff;
			char recvbuf[DEFAULT_BUFFLEN];	

			iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFFLEN, 0);

			mlp.unpackFrame(recvbuf,DEFAULT_BUFFLEN);

			mlp.getFrame(signal, uid, data);

	
		// if new user connects
		if(signal == '0')
		{
			User u1(ClientSocket, data);
			string tmp;

			for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
			{
				if(data == (*it).getUserNickname())
				{
					tmp = "Someone else is using this nickname";
					mlp.fillFrame('3',"Server",tmp.c_str());
					sendbuff = mlp.packFrame();

					send(ClientSocket,sendbuff.c_str(),sendbuff.size()+1,0);
					// Close socket for both, sending and receiving
					shutdown(ClientSocket, SD_BOTH);
					cout << "User tried to connect, but someone else already had the same nickname." << endl;
					goto mainLoop;
				}
			}

			// Adding new user to table of users
			users.push_back(u1);

			// Starts thread that handles incoming data on socket from this user
			cout << "And hes name is: " << (*(--users.end())).getUserNickname().c_str() << endl;
			CreateThread(0,0,handleMessages,const_cast<char *>((*(--users.end())).getUserNickname().c_str()),0,0);
						
			// local message
			cout << " Nickname: " << data << " SOCKET: " << ClientSocket
				 << endl << endl;

			// send info to others
			tmp = data + " connected to the server";
			mlp.fillFrame('1',"Server",tmp.c_str());
			sendbuff = mlp.packFrame();
			

			for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
			{	
				send((*it).getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
			}
		}
	}

	return 0;
}


