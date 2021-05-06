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
// 2021-05-06
//	- execute multi interpreter
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
		bool LoadIntermediateCode(const vector<StrPath> &fileNames);

		bool Process(const float deltaSeconds);
		bool PushEvent(const int itprId, const common::script::cEvent &evt);
		bool Run(const int itprId);
		bool DebugRun(const int itprId);
		bool StepRun(const int itprId);
		bool Stop(const int itprId);
		bool Resume(const int itprId);
		bool OneStep(const int itprId);
		bool Break(const int itprId);
		bool BreakPoint(const int itprId, const bool enable, const uint id);
		bool IsRun(const int itprId);
		bool IsDebug(const int itprId);
		void Clear();


	protected:
		bool SendSyncVMRegister(const int itprId);
		bool SendSyncSymbolTable(const int itprId);
		void ClearInterpreters();

		// remotedbg2 protocol hander
		virtual bool Welcome(remotedbg2::Welcome_Packet &packet) override;
		virtual bool UploadIntermediateCode(remotedbg2::UploadIntermediateCode_Packet &packet) override;
		virtual bool ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet) override;
		virtual bool ReqRun(remotedbg2::ReqRun_Packet &packet) override;
		virtual bool ReqOneStep(remotedbg2::ReqOneStep_Packet &packet) override;
		virtual bool ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet) override;
		virtual bool ReqBreak(remotedbg2::ReqBreak_Packet &packet) override;
		virtual bool ReqBreakPoint(remotedbg2::ReqBreakPoint_Packet &packet) override;
		virtual bool ReqStop(remotedbg2::ReqStop_Packet &packet) override;
		virtual bool ReqInput(remotedbg2::ReqInput_Packet &packet) override;


	public:
		enum class eState { Stop, Run };
		eDebugMode m_mode;

		// interpreter information
		struct sItpr {
			StrId name;
			eState state;
			script::cInterpreter *interpreter;
			vector<uint> insts[10]; // vm instruction index check, max check 10
			vector<bool> cmps[10];
			bool isChangeInstruction;
			float regSyncTime; // register sync time
			float instSyncTime; // instruction sync time
			float symbSyncTime; // symboltable sync time
		};
		vector<sItpr> m_interpreters;

		string m_url; // webserver url
		int m_port; // webserver port
		network2::cWebClient m_client;
		remotedbg2::h2r_Protocol m_protocol;
		script::iFunctionCallback *m_callback;
		void *m_arg;

		// detect change variable
		map<string, sSymbol> m_symbols;
	};

}
