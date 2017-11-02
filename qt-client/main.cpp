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

int initConnectionStructs(const char* ipaddr, struct addrinfo** result, struct addrinfo* hints);

int main(int argc, char** argv)
{
		// qt event loop
		QApplication app(argc, argv);

		// vars to be passed to init window
		// it will fill them with nickname and serverip
		string nick;
		string ip;
		bool initWndReturn = 0;
		struct addrinfo *result;
		struct addrinfo hints;
		SOCKET ConnectSocket = INVALID_SOCKET;

		InitWindow connectWnd(&nick,&ip,&initWndReturn);

getParams:
		connectWnd.show();
		app.exec();

		if(initWndReturn == 0)
				return 1;

		WSADATA wsaData;
		int iResult = 0;

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
		
		do
		{
				cout << "Filling address info . . .";
				iResult = initConnectionStructs(ip.c_str(),&result,&hints);
				if(iResult == WSAHOST_NOT_FOUND)
				{
					cout << "Unknown / invalid server address" << endl;
					MessageBoxA(NULL,
											"Unknown or invalid server address",
											"Error !",
											MB_OK);
					goto getParams;
				}
				if(iResult != 0)
				{
					cout << "Error when initializing conn structs: " << WSAGetLastError() << endl;
					freeaddrinfo(result);
					WSACleanup();
					return 1;
				}

				cout << "Filling socket info . . ." << result;
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
				iResult = connect( ConnectSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
				if(iResult == SOCKET_ERROR)
				{
					int wannaRetry;
					string errorMsg("Cannot connect to the server ");
					errorMsg += ip;
					wannaRetry = MessageBoxA(NULL,
																	errorMsg.c_str(),
																	"Error !",
																	MB_RETRYCANCEL);

					cout << "Error at connect(): " << WSAGetLastError() << endl;
					if(wannaRetry == IDRETRY)
					{
						cout << "Retrying . . ." << endl;
						freeaddrinfo(result);
						continue;
					}
					connectWnd.show();
					app.exec();
					
					// user closed window
					if(initWndReturn == 0)
					{
						freeaddrinfo(result);
						WSACleanup();
						return 1;
					}

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

		WaitForSingleObject(forkReceive, 500);
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
					shutdown(ConnectSocket,SD_BOTH);

			goto getParams;
		}


		CloseHandle(forkReceive);
		delete forkReceiveReturn;
		// Closes socket for both receiving and sending

		return 0;
}

int initConnectionStructs(const char* ipaddr, struct addrinfo** result, struct addrinfo* hints)
{
		ZeroMemory(hints, sizeof(addrinfo));

		hints->ai_family = AF_INET; // IPv4
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		return getaddrinfo(ipaddr, DEFAULT_PORT, hints, result);
}
