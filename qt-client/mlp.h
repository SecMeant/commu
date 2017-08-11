// Message Switcher Light Protocol
#ifndef _MLP_H
#define _MLP_H

#include <string>
#include <iostream>

using std::string;

struct mlpFrame
{
    char signal;
    string uid;
    string data;
};


class mlProto
{
    private: mlpFrame frame;
   
    public:
    mlProto();
    mlProto(char t, string nick, string msg);
    string packFrame();
    void unpackFrame(string pf);
    void fillFrame(char t, string nick, string msg);
    void getFrame(char & t, string & nick, string & msg);
};

#endif // _MLP_H
