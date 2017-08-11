#ifndef _THREADFUNCTIONS_H
#define _THREADFUNCTIONS_H
#include <vector>
#include <string>
#include <windows.h>

using namespace std;

#ifdef _MESWCS_

DWORD WINAPI recvOverCS(void * uSock)
{
	// Alocation of some basic vars
	char signal;
    string uid, data;
    mlProto mlp;
	int iResult;
	SOCKET ConnectSocket = *((SOCKET *)uSock);	
	char recvbuff[DEFAULT_BUFFLEN];

	while(true)
	{
		iResult = recv(ConnectSocket, recvbuff, DEFAULT_BUFFLEN, 0);
		if(iResult == SOCKET_ERROR)
			break;
		
		mlp.unpackFrame(recvbuff);

		mlp.getFrame(signal, uid, data);
		
		switch(signal)
		{
			case '1':
				cout << uid << ": " << data << endl;
				break;
			case '2':
				cout << "***** " << uid << ": " << data  << " *****" << endl;
				break;
			case '3':
				cout << "U have been kicked from server" << endl
					 << "reason: " << data << endl;
				shutdown(ConnectSocket, SD_BOTH);
				break;
			case '4':
				cout << "U have been banned from server" << endl
					 << "reason: " << data << endl;
				shutdown(ConnectSocket, SD_BOTH);
				break;
			case '5':
				cout << "Status: " << data << endl;
				break;
			case '6':
				cout << "PRIVATE FROM " << uid << ": " << data << endl;
				break;
			default:
				cout << "Unknown signal received" << endl;
				break;
		}
	}

	return 0;
}

#endif

#ifdef _MESWSS_
extern std::vector<User> users;

DWORD WINAPI recvOverSS(void * ud)
{
	// Alocation of some basic vars
	char signal;
    string uid, data;
    mlProto mlp;
	int iResult;
	SOCKET ConnectSocket = ((User *)ud)->getUserSock();	
	char recvbuff[DEFAULT_BUFFLEN];
	string sendbuff;

	while(true)
	{
		iResult = recv(ConnectSocket, recvbuff, DEFAULT_BUFFLEN, 0);
		if(iResult == SOCKET_ERROR)
			break;
		
		mlp.unpackFrame(recvbuff);

		mlp.getFrame(signal, uid, data);
		
		switch(signal)
		{
			case '1':
				// show localy
				cout << ((User *)ud)->getUserNickname() << ": " << data << endl;
				
				// send to others
				for(int i=0; i < users.size(); i++)
				{	
					mlp.fillFrame('1',((User *)ud)->getUserNickname(),data);
					sendbuff = mlp.packFrame();
					send(users[i].getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
				}
				break;
			case '2':
				// show localy
				cout << "***** " << uid << ": " << data  << " *****" << endl;

				// send to others
				for(int i=0; i < users.size(); i++)
				{	
					mlp.fillFrame('2',uid,data);
					sendbuff = mlp.packFrame();
					send(users[i].getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
				}
				break;
			case '6':
				for(int i=0; i < users.size(); i++)
				{
					if(users[i].getUserNickname() == uid)
					{
						mlp.fillFrame('6',((User *)ud)->getUserNickname(),data);
						sendbuff = mlp.packFrame();
						send(users[i].getUserSock(),sendbuff.c_str(),sendbuff.size()+1,0);
					}
				}
				break;
			default:
				cout << "Unknown signal received" << endl;
				break;
		}
	}

	// send info to others that user closed connection
	string tmp = ((User *)ud)->getUserNickname() + " disconnected";
	mlp.fillFrame('5',"Server",tmp.c_str());
	sendbuff = mlp.packFrame();

	for(int i=0; i < users.size(); i++)
	{	
		send(users[i].getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
	}
	cout << "closing " << ((User *)ud)->getUserNickname() << " socket" << endl;
	return 0;
}

#endif


#endif
