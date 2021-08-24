//------------------------------------------------------------------------
// Name:    visualprogram_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace visualprogram {

using namespace network2;
using namespace marshalling_json;
static const int s2w_Protocol_ID = 2100;

class s2w_Protocol : public network2::iProtocol
{
public:
	s2w_Protocol() : iProtocol(s2w_Protocol_ID, ePacketFormat::JSON) {}
	void AckWebLogin(netid targetId, bool isBinary, const string &id, const int &result);
	void AckWebRunVisualProgStream(netid targetId, bool isBinary, const string &nodeFileName, const int &result);
	void AckWebStopVisualProg(netid targetId, bool isBinary, const int &result);
	void ReqWebConnectRIProxyServer(netid targetId, bool isBinary, const string &url, const string &name);
};
static const int w2s_Protocol_ID = 2101;

class w2s_Protocol : public network2::iProtocol
{
public:
	w2s_Protocol() : iProtocol(w2s_Protocol_ID, ePacketFormat::JSON) {}
	void ReqWebLogin(netid targetId, bool isBinary, const string &id);
	void ReqWebRunVisualProgStream(netid targetId, bool isBinary, const string &nodeFileName);
	void ReqWebStopVisualProg(netid targetId, bool isBinary);
};
static const int s2c_Protocol_ID = 1000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
	void Welcome(netid targetId, bool isBinary, const string &msg);
	void AckLogin(netid targetId, bool isBinary, const string &id, const int &result);
	void ReqRunVisualProg(netid targetId, bool isBinary, const webvpl::sNodeFile &nodeFile);
	void ReqRunVisualProgStream(netid targetId, bool isBinary, const uint &count, const uint &index, const vector<BYTE> &data);
	void ReqRunVisualProgFiles(netid targetId, bool isBinary, const vector<string> &nodeFileNames);
	void ReqStopVisualProg(netid targetId, bool isBinary);
	void ReqConnectRIProxyServer(netid targetId, bool isBinary, const string &url, const string &name);
};
static const int c2s_Protocol_ID = 2000;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void ReqLogin(netid targetId, bool isBinary, const string &id);
	void AckRunVisualProg(netid targetId, bool isBinary, const int &result);
	void AckRunVisualProgStream(netid targetId, bool isBinary, const int &result);
	void AckRunVisualProgFiles(netid targetId, bool isBinary, const int &result);
	void AckStopVisualProg(netid targetId, bool isBinary, const int &result);
	void AckConnectRIProxyServer(netid targetId, bool isBinary, const string &url, const string &name, const int &result);
};
}
