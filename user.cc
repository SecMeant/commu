#include <cstring> // strncpy
#include <winsock2.h>
#include <iostream>
#include <string>

#ifndef _MLP_CC
#define _MLP_CC
#include "mlp.cc"
#endif

using std::cout;
using std::endl;
using std::string;

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "31337"
#endif

#ifndef DEFAULT_BUFFLEN
#define DEFAULT_BUFFLEN 1024
#endif

typedef unsigned int uint;

class User
{
#define BUFFSIZE 512
#define NICKSIZE 64
private:
    static uint counter;
    uint id;
    SOCKET sock;
    string recvbuffer;
    string sendbuffer;
    string nick;
public:
    User()
    :id(this->counter++), sock(INVALID_SOCKET), recvbuffer(""), sendbuffer(""), nick("Noname"){}

    User(const SOCKET &s, const string & n)
    :id(this->counter++), sock(s), recvbuffer(""), sendbuffer(""), nick(n) {}

    uint getUserId();
    string getUserNickname();
	int getUserSock();

    void setSocket(const SOCKET &s);
    void setNickname(const string &s);

    // If socket == INVALID_SOCKET then this->sock wont change
    // Same goes with nick if string is empty ^
    void fillUserInfo(const SOCKET &s, const string &n);

    void show();

    //bool forkReceive();
};

uint User::getUserId()
{
    return this->id;
}

string User::getUserNickname()
{
    return this->nick;
}

int User::getUserSock()
{
	return this->sock;
}

void User::setSocket(const SOCKET &s)
{
    this->sock = s;
    return;
}

void User::setNickname(const string &s)
{
    this->nick = s;
    return;
}

void User::fillUserInfo(const SOCKET &s, const string &n)
{
    if(s != INVALID_SOCKET)
        this->sock = s;
    if(n != "")
        this->nick = n;
    return;
}

void User::show()
{
	cout << "counter: " << this->counter << endl;
	cout << "id: " << this->id << endl;
	if(sock != INVALID_SOCKET)
		cout << "sock: " << "ESTABLISHED" << endl;
	else
		cout << "sock: " << "INVALID" << endl;
	cout << "nick: " << nick << endl;
}
