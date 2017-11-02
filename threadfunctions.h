#ifndef _THREADFUNCTIONS_H
#define _THREADFUNCTIONS_H
#include <list>
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
		
		mlp.unpackFrame(recvbuff,DEFAULT_BUFFLEN);

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

#endif // _MESWCS_

#ifdef _MESWSS_
extern std::list<User> users;

DWORD WINAPI handleMessages(void * ud)
{
	// Alocation of some basic vars
	char signal;
	string uid, data;
	mlProto mlp;
	list<User>::iterator handledUser = users.begin();
	int iResult;

	while((*handledUser).getUserNickname() != reinterpret_cast<char *>(ud))
		handledUser++;

	SOCKET ConnectSocket = (*(handledUser)).getUserSock();	
	cout << "Connect Socket inner: " << ConnectSocket << endl;
	char recvbuff[DEFAULT_BUFFLEN]={};
	string sendbuff;

	while(true)
	{
		iResult = recv(ConnectSocket, recvbuff, DEFAULT_BUFFLEN-1, 0);
		if(iResult == SOCKET_ERROR || iResult <= 0)
		{
			cout << "SOCKET ERROR" << endl;
			break;
		}
		
		mlp.unpackFrame(recvbuff,DEFAULT_BUFFLEN);

		mlp.getFrame(signal, uid, data);
		
		switch(signal)
		{
			// Normal message to everyone
			case '1':
				// show localy
				cout << (*handledUser).getUserNickname() << ": " << data << endl;
				
				// send to others
				for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
				{	
					mlp.fillFrame('1',(*(handledUser)).getUserNickname(),data);
					sendbuff = mlp.packFrame();
					send((*it).getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
				}
				break;
			// Important message to everyone
			case '2':
				// show localy
				out << "***** " << uid << ": " << data  << " *****" << endl;

				// send to others
				for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
				{	
					mlp.fillFrame('2',uid,data);
					sendbuff = mlp.packFrame();
					send((*it).getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
				}
				break;
			
			case '6':
				// Private message
				// show localy
				cout  << "PW From: " << (*(handledUser)).getUserNickname()
							<< " To: " << uid << "Message: " << data << endl;
				for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
				{
					if((*it).getUserNickname() == uid)
					{
						mlp.fillFrame('6',(*(handledUser)).getUserNickname(),data);
						sendbuff = mlp.packFrame();
						send((*it).getUserSock(),sendbuff.c_str(),sendbuff.size()+1,0);
					}
				}
				break;
			default:
				cout << "Unknown or illegal signal received" << endl;
				break;
		}
	}

	// send info to others that user closed connection
	string tmp = (*(handledUser)).getUserNickname() + " disconnected";
	mlp.fillFrame('5',"Server",tmp.c_str());
	sendbuff = mlp.packFrame();

	cout << "closing " << (*(handledUser)).getUserNickname() << " socket" << endl;
	
	users.erase(handledUser);

	for(list<User>::iterator it = users.begin(); it != users.end(); ++it)
	{	
		send((*it).getUserSock(),sendbuff.c_str(),sendbuff.size()+1, 0);
	}
	

	return 0;
}

#endif // _MESWSS_


#endif // _THREADFUNCTIONS_H
