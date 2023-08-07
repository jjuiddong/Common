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

		enum class eState { Stop, Run };

		// interpreter information
		struct sItpr {
			StrId name;
			eState state;
			script::cInterpreter* interpreter;
			vector<ushort> insts[10]; // vm instruction index array, max vm count:10
			bool isChangeInstruction;
			float regSyncTime; // register sync time, seconds unit
			float instSyncTime; // instruction sync time, seconds unit
			float symbSyncTime; // symboltable sync time, seconds unit

			map<string, sSymbol> chSymbols; // check change variable
											// key: variable name = scopeName + varname
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
		bool PushEvent(const int itprId, const int vmId, const common::script::cEvent &evt);
		bool Run(const int itprId);
		bool DebugRun(const int itprId);
		bool StepRun(const int itprId);
		bool Stop(const int itprId);
		bool Resume(const int itprId);
		bool ResumeVM(const int itprId, const string &vmName);
		bool OneStep(const int itprId);
		bool Break(const int itprId);
		bool BreakPoint(const int itprId, const int vmId, const bool enable, const uint id);
		bool SyncInformation(const int itprId, const int vmId);
		bool SyncVMInformation(const int vmId, const vector<string>& varNames);
		bool IsRun(const int itprId);
		bool IsDebug(const int itprId);
		bool IsConnect();
		bool IsFailConnect();
		void ClearInterpreters();
		void Clear();

		script::cVirtualMachine* GetVM(const int vmId);


	protected:
		sItpr* GetInterpreterByVMId(const int vmId);
		int GetInterpreterIdByVMId(const int vmId);
		bool RunInterpreter(const int itprId, const int type);
		bool RunInterpreter_Sub(const int itprId, const int type);
		bool IsChangeSymbolTable(const int itprId);
		bool IsChangeSymbolTable(const sItpr &itpr);
		bool SendSyncVMRegister(const int itprId, const set<int> *vmIds = nullptr);
		bool SendSyncInstruction(const int itprId);
		bool SendSyncSymbolTable(const int itprId, const int vmId);
		bool SendSyncVariable(const int itprId, const int vmId
			, const script::cSymbolTable &symbolTable
			, const string& varName, const script::sVariable &var);

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
		virtual bool ReqChangeVariable(remotedbg2::ReqChangeVariable_Packet& packet) override;


	public:
		vector<sItpr> m_interpreters;
		vector<script::iModule*> m_modules;

		bool m_isThreadMode;
		int m_bindPort; // webserver bind port
		network2::cWebServer m_server; // interpreter host server
		remotedbg2::h2r_Protocol m_protocol;
		set<int> m_syncVMIds; // sync debug info virtual machine id
		common::cTPSemaphore *m_threads; // thread pool reference
		std::atomic<int> m_multiThreading; // multithread work?, 0:no threading work
	};

}
