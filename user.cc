#include <cstring> // strncpy
#include <winsock2.h>
#include <iostream>

using std::cout;
using std::endl;

class User
{
#define uint unsigned int
#define BUFFSIZE 512
#define NICKSIZE 64
private:
    static uint counter;
    uint id;
    SOCKET sock;
    char buffer[BUFFSIZE];
    char nick[NICKSIZE];
public:
    User() : sock(INVALID_SOCKET)
    {
        for(int i = 0; i < BUFFSIZE; i++)
            this->buffer[i] = '\0';
        
        for(int i = 0; i < NICKSIZE; i++)
            this->nick[i] = '\0';
    }

    User(SOCKET &s, const char * n, const uint nsize)
    :id(this->counter++), sock(s)
    {     
        for(int i = 0; i < BUFFSIZE; i++)
            this->buffer[i] = '\0';

        if(nsize >= NICKSIZE)
            for(int i = 0; i < NICKSIZE; i++)
                this->nick[i] = '\0';
        else
            strncpy(this->nick,n,nsize);
    }

    void ShowAll()
    {
        
        cout << "User: " << this->nick << endl;
        cout << "id: " << this->id << endl;
    }
    // Spawns thread which runs checkSendMsg()
    //bool initiate();

    // Checks if user send any data
    // If so send the data to others
    //void checkSendMsg(SOCKET * users, uint usercount);

};
