//
// 2020-11-20, jjuiddong
// RemoteDebugger with WebServer
//	- synchronize debug information with webserver
//	- now only Host Mode
//		- connect webserver
//		- remote debugger is webclient
//	- todo:
//		- remote work
//		- webserver work
//
#pragma once

#include "Src/remotedbg2_Protocol.h"
#include "Src/remotedbg2_ProtocolHandler.h"


namespace network2
{

	class cRemoteDebugger2 : public remotedbg2::r2h_ProtocolHandler
	{
	public:
		enum class eDebugMode { None, Host, Remote };

		cRemoteDebugger2();
		virtual ~cRemoteDebugger2();

		bool InitHost(const string &url, const int port);
		bool InitRemote(const Str16 &ip, const int port);
		bool Process();
		bool Run();
		bool Stop();
		bool OneStep();
		bool Break();
		bool Terminate();
		bool IsRun();
		void Clear();


	protected:
		// remotedbg2::r2h_ProtocolHandler protocol handler
		virtual bool UploadVProgFile(remotedbg2::UploadVProgFile_Packet &packet) override;
		virtual bool ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet) override;
		virtual bool ReqRun(remotedbg2::ReqRun_Packet &packet) override;
		virtual bool ReqOneStep(remotedbg2::ReqOneStep_Packet &packet) override;
		virtual bool ReqBreak(remotedbg2::ReqBreak_Packet &packet) override;
		virtual bool ReqStop(remotedbg2::ReqStop_Packet &packet) override;
		virtual bool ReqInput(remotedbg2::ReqInput_Packet &packet) override;


	protected:
		enum class eState { Stop, Run };
		eState m_state;
		eDebugMode m_mode;

		script::cInterpreter m_interpreter;
		script::cDebugger m_debugger;
		script::cIntermediateCode m_icode;

		string m_url; // webserver url
		int m_port; // webserver port
		cNetController m_netController;
		network2::cWebClient m_client;
		remotedbg2::h2r_Protocol m_protocol;
		cTimer m_timer;
		float m_syncTime;
	};

}
