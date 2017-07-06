class User
{
#define uint unsigned int
#define BUFFSIZE 512
#define NICKSIZE 64
private:
    uint id;
    SOCKET sock;
    char buffer[BUFFSIZE];
    char nick[NICKSIZE];
public:
    User() : id(0), sock(INVALID_SOCKET)
    {
        for(int i = 0; i < BUFFSIZE; i++)
            this->buffer[i] = '\0';
        
        for(int i = 0; i < NICKSIZE; i++)
            this->nick[i] = '\0';
    }

    User(uint i, SOCKET s, char * b, uint bsize, char * n, uint nsize)
    :id(i), SOCKET(s)
    {
        if(bsize >= BUFFSIZE)
            for(int i = 0; i < BUFFSIZE; i++)
                this->buffer[i] = '\0';
        else
            strncpy(this->buffer,b,bsize);

        if(nsize >= NICKSIZE)
            for(int i = 0; i < NICKSIZE; i++)
                this->nick[i] = '\0';
        else
            strncpy(this->nick,n,nsize);
    }
    // Spawnuje thread ktory odpala checkSendMsg()
    bool initiate();

    // Sprawdza czy user wyslal jakies dane
    // Jezeli tak to rozsyla to do reszty
    void checkSendMsg(SOCKET * users, uint usercount);

}
