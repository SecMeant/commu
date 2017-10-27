// qt libs
#include <QApplication>
#include <QMessageBox>
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
		bool initWndReturn;

		InitWindow connectWnd(&nick,&ip,&initWndReturn);

getParams:
		connectWnd.show();
		app.exec();

		if(initWndReturn == 0)
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

		// This block will destroy errorMsg string and
		{
			int wannaRetry;
			string errorMsg("Cannot connect to the server ");
			errorMsg += ip;
			do
			{
					cout << "Connecting to the server. . .";
					iResult = connect( ConnectSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
					if(iResult == SOCKET_ERROR)
					{
							wannaRetry = MessageBoxA(NULL,
																			errorMsg.c_str(),
																			"Error !",
																			MB_RETRYCANCEL);

							cout << "Error at connect(): " << WSAGetLastError() << endl;
							if(wannaRetry == IDRETRY)
							{
								cout << "Retrying . . ." << endl;
								continue;
							}
							connectWnd.show();
							app.exec();
							
							if(initWndReturn == 0)
									return 1;
					}
			}while(iResult == SOCKET_ERROR);
		}
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
		send(ConnectSocket, sendbuff.c_str(), static_cast<int>(sendbuff.size()+1) , 0);

		ChatWindow w(ConnectSocket);
		w.show();

		// forking recviving thread
		// sending address of ConnectSocket is ok because
		// only one connection is established on client side
		// Also thread is responsible of closing chatWindow when server
		// closes connection
		int * forkReceiveReturn = new int;
		HANDLE forkReceive = CreateThread(0,0,recvOverCS,(void*)&w,0,0);

		app.exec();

		WaitForSingleObject(forkReceive, INFINITE);
		GetExitCodeThread(forkReceive, reinterpret_cast<LPDWORD>(forkReceiveReturn));

		// 1 - Lost connection
		// 0 - Closed properly
		if(*forkReceiveReturn)
		{
			MessageBoxA(
					NULL, // dont inherit
					"Connection lost ;/",
					"Oops !",
					MB_OK);

			goto getParams;
		}

		CloseHandle(forkReceive);
		delete forkReceiveReturn;

		return 0;
}
