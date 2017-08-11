#include "threadfunctions.h"
#include "chatwindow.h"

DWORD WINAPI recvOverCS(void * w)
{
    // Alocation of some basic vars
    char signal;
    string uid, data;
    mlProto mlp;
    int iResult;

    ChatWindow* wnd = (ChatWindow*)w;
    SOCKET ConnectSocket = wnd->getSock();
    char recvbuff[DEFAULT_BUFFLEN];

    while(true)
    {
        iResult = recv(ConnectSocket, recvbuff, DEFAULT_BUFFLEN, 0);
        if(iResult == SOCKET_ERROR)
            break;
        //wnd->chat_label->setText("asdf");

        mlp.unpackFrame(recvbuff);

        mlp.getFrame(signal, uid, data);

        switch(signal)
        {
            case '1':
                *wnd << uid << ": " << data << "\n";
                break;
            case '2':
                *wnd << "***** " << uid << ": " << data  << " *****" << "\n";
                break;
            case '3':
                *wnd << "U have been kicked from server" << "\n"
                     << "reason: " << data << "\n";
                shutdown(ConnectSocket, SD_BOTH);
                break;
            case '4':
                *wnd << "U have been banned from server" << "\n"
                     << "reason: " << data << "\n";
                shutdown(ConnectSocket, SD_BOTH);
                break;
            case '5':
                *wnd << "Status: " << data << "\n";
                break;
            case '6':
                *wnd << "PRIVATE FROM " << uid << ": " << data << "\n";
                break;
            default:
                *wnd << "Unknown signal received" << "\n";
                break;
        }
    }

    return 0;
}
