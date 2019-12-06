//------------------------------------------------------------------------
// Name:    basic_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace basic {

using namespace network2;
using namespace marshalling;
static const int s2c_Protocol_ID = 200;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID) {}
	void func1(netid targetId);
	void func2(netid targetId, const std::string &str);
	void func3(netid targetId, const float &value);
	void func4(netid targetId);
	void func5(netid targetId, const string &name, const vector<int> &ar);
	void func6(netid targetId, const vector<int> &ar);
};
static const int c2s_Protocol_ID = 300;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID) {}
	void func5(netid targetId, const std::string &str);
	void func6(netid targetId, const float &value);
};
}
