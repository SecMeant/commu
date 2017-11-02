// Message Switcher Light Protocol

#include <string>
#include <iostream>

typedef unsigned int uint;

using std::string;

struct mlpFrame
{
	char signal;
	string uid;
	string data;
};

class mlProto
{
private: 
	mlpFrame frame;
	 
public:
	mlProto();
	mlProto(char t, string nick, string msg);
	string packFrame();
	void unpackFrame(string pf, uint maxlen);
	void fillFrame(char t, string nick, string msg);
	void getFrame(char & t, string & nick, string & msg);
};

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

void mlProto::unpackFrame(string pf, uint maxlen)
{
	this->frame.signal = pf[0];
	this->frame.uid = "";
	this->frame.data = "";
	int i=1;
	for(; pf[i] != 0x18 && i < maxlen; i++)
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
