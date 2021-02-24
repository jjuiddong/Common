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
// 2021-02-24
//	- change variable synchronize
//	- streaming
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

		// synchronize symbol to check change variable
		// only change variable synchroinizing
		struct sSymbol {
			StrId name; // symbol name
			float t; // sync time
			common::script::sVariable var; // compare variable
		};

		cRemoteDebugger2();
		virtual ~cRemoteDebugger2();

		bool InitHost(cNetController &netController
			, const string &url
			, script::iFunctionCallback *callback = nullptr
			, void *arg = nullptr
		);
		bool InitRemote(cNetController &netController
			, const Str16 &ip, const int port);
		bool LoadIntermediateCode(const StrPath &fileName);
		bool Process(const float deltaSeconds);
		bool PushEvent(const common::script::cEvent &evt);
		bool Run();
		bool StepRun();
		bool Stop();
		bool Resume();
		bool OneStep();
		bool Break();
		bool IsRun();
		bool IsDebug();
		void Clear();


	protected:
		bool SendSyncVMRegister();
		bool SendSyncSymbolTable();

		// remotedbg2 protocol hander
		virtual bool Welcome(remotedbg2::Welcome_Packet &packet) override;
		virtual bool UploadVProgFile(remotedbg2::UploadVProgFile_Packet &packet) override;
		virtual bool ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet) override;
		virtual bool ReqRun(remotedbg2::ReqRun_Packet &packet) override;
		virtual bool ReqOneStep(remotedbg2::ReqOneStep_Packet &packet) override;
		virtual bool ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet) override;
		virtual bool ReqBreak(remotedbg2::ReqBreak_Packet &packet) override;
		virtual bool ReqStop(remotedbg2::ReqStop_Packet &packet) override;
		virtual bool ReqInput(remotedbg2::ReqInput_Packet &packet) override;


	public:
		enum class eState { Stop, Run };
		eState m_state;
		eDebugMode m_mode;

		script::cInterpreter m_interpreter;
		script::cDebugger m_debugger;
		script::cIntermediateCode m_icode;
		vector<uint> m_insts[10]; // vm instruction index check, max check 10
		vector<bool> m_cmps[10];
		bool m_isChangeInstruction;

		string m_url; // webserver url
		int m_port; // webserver port
		network2::cWebClient m_client;
		remotedbg2::h2r_Protocol m_protocol;
		float m_regSyncTime; // register sync time
		float m_instSyncTime; // instruction sync time
		float m_symbSyncTime; // symboltable sync time

		// detect change variable
		map<string, sSymbol> m_symbols;
	};

}
