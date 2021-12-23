//
// 2021-09-23, jjuiddong
// RemoteInterpreter
//	- run intermediate code with interpreter
//	- remote interpreter is Server (WebSocket Server)
//	- communication with remote debugger (client)
//	- synchronize with remote debugger
//  - naming change RemoteDebugger2 -> RemoteInterpreter
//
#pragma once

#include "Src/remotedbg2_Protocol.h"
#include "Src/remotedbg2_ProtocolHandler.h"


namespace network2
{

	class cRemoteInterpreter : public remotedbg2::r2h_ProtocolHandler
	{
	public:
		// synchronize changed variable
		struct sSymbol {
			StrId name; // symbol name
			float t; // sync time
			common::script::sVariable var; // compare variable
		};

		cRemoteInterpreter(const StrId &name, const int logId = -1);
		virtual ~cRemoteInterpreter();
		bool Reuse(const StrId &name, const int logId);

		bool Init(cNetController &netController
			, const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
			, const bool isSpawnHttpSvr = true
		);
		bool LoadIntermediateCode(const StrPath &fileName);
		bool LoadIntermediateCode(const vector<StrPath> &fileNames);
		bool LoadIntermediateCode(const common::script::cIntermediateCode &icode);
		bool AddModule(common::script::iModule *mod);
		bool RemoveModule(common::script::iModule *mod);
		bool SendSyncAll();

		bool Process(const float deltaSeconds);
		bool PushEvent(const int itprId, const common::script::cEvent &evt);
		bool Run(const int itprId);
		bool DebugRun(const int itprId);
		bool StepRun(const int itprId);
		bool Stop(const int itprId);
		bool Resume(const int itprId);
		bool ResumeVM(const int itprId, const string &vmName);
		bool OneStep(const int itprId);
		bool Break(const int itprId);
		bool BreakPoint(const int itprId, const bool enable, const uint id);
		bool IsRun(const int itprId);
		bool IsDebug(const int itprId);
		bool IsConnect();
		bool IsFailConnect();
		void ClearInterpreters();
		void Clear();


	protected:
		bool SendSyncVMRegister(const int itprId);
		bool SendSyncInstruction(const int itprId);
		bool SendSyncSymbolTable(const int itprId);

		// remotedbg2 protocol hander
		virtual bool UploadIntermediateCode(remotedbg2::UploadIntermediateCode_Packet &packet) override;
		virtual bool ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet) override;
		virtual bool ReqRun(remotedbg2::ReqRun_Packet &packet) override;
		virtual bool ReqOneStep(remotedbg2::ReqOneStep_Packet &packet) override;
		virtual bool ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet) override;
		virtual bool ReqBreak(remotedbg2::ReqBreak_Packet &packet) override;
		virtual bool ReqBreakPoint(remotedbg2::ReqBreakPoint_Packet &packet) override;
		virtual bool ReqStop(remotedbg2::ReqStop_Packet &packet) override;
		virtual bool ReqInput(remotedbg2::ReqInput_Packet &packet) override;
		virtual bool ReqEvent(remotedbg2::ReqEvent_Packet &packet) override;
		virtual bool ReqStepDebugType(remotedbg2::ReqStepDebugType_Packet &packet) override;
		virtual bool ReqDebugInfo(remotedbg2::ReqDebugInfo_Packet &packet) override;


	public:
		enum class eState { Stop, Run };

		// interpreter information
		struct sItpr {
			StrId name;
			eState state;
			script::cInterpreter *interpreter;
			vector<ushort> insts[10]; // vm instruction index array, max vm count:10
			bool isChangeInstruction;
			float regSyncTime; // register sync time, seconds unit
			float instSyncTime; // instruction sync time, seconds unit
			float symbSyncTime; // symboltable sync time, seconds unit
		};
		vector<sItpr> m_interpreters;
		vector<script::iModule*> m_modules;

		bool m_isThreadMode;
		int m_bindPort; // webserver bind port
		network2::cWebServer m_server;
		remotedbg2::h2r_Protocol m_protocol;
		int m_symbolTableSyncItprId; // symbol table synchronize interpreter id
		set<int> m_syncItptrs; // sync debug info interpreter id
		map<string, sSymbol> m_chSymbols; // check change variable
										  // key: variable name = scopeName + varname
		common::cTPSemaphore *m_threads; // thread pool reference
		std::atomic<int> m_multiThreading; // multithread work?, 0:no threading work
	};

}
