#ifndef _THREADFUNCTIONS_H
#define _THREADFUNCTIONS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <vector>
#include <string>

#include <QMessageBox>
#include <QThread>

#include "mlp.h"

#define DEFAULT_BUFFLEN 1024

using namespace std;

#ifdef _MESWCS_

DWORD WINAPI recvOverCS(void * uSock);

#endif // MESWCS

#endif // THREADFUNCTIONS_H

