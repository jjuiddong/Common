//
// 2021-11-30, jjuiddong
// protocol generator config
//
// config file sample
//{
//"source":
//[
//	"webrelay.prt",
//	"cluster.prt",
//	"remotedbg2.prt",
//	"vplp.prt"
//],
//"dispatcher":
//[
//	"webrelay.s2c_Dispatcher",
//	"remotedbg2.h2r_Dispatcher",
//	"vplp.s2c_Dispatcher",
//	"cluster.c2s_Dispatcher",
//	"cluster.s2c_Dispatcher"
//],
//"handler":
//[
//	"webrelay.s2c_ProtocolHandler",
//	"remotedbg2.h2r_ProtocolHandler",
//	"vplp.s2c_ProtocolHandler",
//	"cluster.s2c_ProtocolHandler"
//],
//"output": "output.js"
//}
//
#pragma once


class cCfg
{
public: 
	cCfg(const string &fileName = "");
	virtual ~cCfg();

	bool Parse(const string &fileName);
	bool IsValid();
	void Clear();


public:
	vector<string> m_sources; // source filename
	vector<string> m_dispatchers; // dispatcher class name
	vector<string> m_handlers; // handler class name
	string m_output;  // output filename
};
