#include "mlp.h"

mlProto::mlProto()
{
    fillFrame('0', "", "");
}

mlProto::mlProto(char t, string nick, string msg)
{
    fillFrame(t, nick, msg);
}

string mlProto::packFrame()
{
    string r1;
    r1 += this->frame.signal;
    r1 += this->frame.uid;
    r1 += 0x18;
    r1 += this->frame.data;

    return r1;

}

void mlProto::unpackFrame(string pf)
{
    this->frame.signal = pf[0];
    this->frame.uid = "";
    this->frame.data = "";
    int i=1;
    for(; pf[i] != 0x18; i++)
    {
        this->frame.uid += pf[i];
    }
    pf.erase(0,i+1);
    this->frame.data = pf;

    return;
}

void mlProto::fillFrame(char t, string nick, string msg)
{
    this->frame.signal = t;
    this->frame.uid = nick;
    this->frame.data = msg;

    return;
}

void mlProto::getFrame(char & t, string & nick, string & msg)
{
    t = this->frame.signal;
    nick = this->frame.uid;
    msg = this->frame.data;

    return;
}
