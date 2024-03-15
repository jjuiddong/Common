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
							  , public common::script::iInterpreterResponse
	{
	public:
		// interpreter information
		struct sItpr 
		{
			enum class eState { Stop, Run };

			string name;
			eState state;
			script::cInterpreter* interpreter;
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
		int LoadIntermediateCode(const StrPath &fileName
			, const int parentVmId = -1
			, const string& scopeName = "");
		bool LoadIntermediateCode(const vector<StrPath> &fileNames
			, const int parentVmId = -1
			, const string& scopeName = "");
		int LoadIntermediateCode(const common::script::cIntermediateCode &icode
			, const int parentVmId = -1
			, const string& scopeName = "");
		int AddVM(const int itprId, const common::script::cIntermediateCode& icode
			, const int parentVmId = -1
			, const string& scopeName = "");
		bool AddModule(common::script::iModule *mod);
		bool RemoveModule(common::script::iModule *mod);
		bool SendSyncAll();

		bool Process(const float deltaSeconds, const uint procCnt = 1);
		bool PushEvent(const int itprId, const int vmId, shared_ptr<script::cEvent> evt);
		bool Run(const int itprId, const int parentVmId = -1, const int vmId = -1
			, const script::cSymbolTable& symbTable = {}
			, const string& nodeName = ""
			, const string& startEvent = "Start Event");
		bool DebugRun(const int itprId, const int parentVmId = -1, const int vmId = -1
			, const script::cSymbolTable& symbTable = {}
			, const string& nodeName = ""
			, const string& startEvent = "Start Event");
		bool StepRun(const int itprId, const int parentVmId = -1, const int vmId = -1
			, const script::cSymbolTable& symbTable = {}
			, const string& nodeName = ""
			, const string& startEvent = "Start Event");
		bool Stop(const int itprId, const int vmId = -1);
		bool TerminateVM(const int vmId);
		bool Resume(const int itprId, const int vmId = -1);
		bool OneStep(const int itprId, const int vmId = -1);
		bool Break(const int itprId, const int vmId = -1);
		bool BreakPoint(const int itprId, const int vmId, const bool enable, const uint id);
		bool SyncInformation(const int itprId, const int vmId);
		bool SyncVMInformation(const int vmId, const vector<string>& varNames);
		bool IsRunning(const int itprId);
		bool IsRun(const int itprId);
		bool IsDebugRun(const int itprId);
		bool IsDebugging(const int itprId);
		bool IsConnect();
		bool IsFailConnect();
		void ClearInterpreters();
		void Clear();

		script::cVirtualMachine* GetVM(const int vmId);
		script::cVirtualMachine* GetRemoveVM(const int vmId);

		// iInterpreterResponse handler
		virtual void TerminateResponse(const int vmId) override;
		virtual void SetTimeOutResponse(const int vmId, const string& scopeName, const int timerId, const int time) override;
		virtual void ClearTimeOutResponse(const int vmId, const int timerId, const int id) override;
		virtual void SetIntervalResponse(const int vmId, const string& scopeName, const int timerId, const int time) override;
		virtual void ClearIntervalResponse(const int vmId, const int timerId, const int id) override;
		virtual void SyncTimeOutResponse(const int vmId, const string& scopeName, const int timerId, const int time) override;
		virtual void ClearSyncTimeOutResponse(const int vmId, const int timerId, const int id) override;
		virtual void ErrorVM(const int vmId, const string &msg) override;
		//~


	protected:
		sItpr* GetInterpreterByVMId(const int vmId);
		int GetInterpreterIdByVMId(const int vmId);
		int GetInterpreterIdByRemoveVMId(const int vmId);
		bool RunInterpreter(const int itprId, const int parentVmId
			, const int vmId, const script::cSymbolTable& symbTable
			, const string &nodeName, const string& startEvent, const int type);
		bool RunInterpreter_Sub(const int itprId, const int parentVmId
			, const int vmId, const script::cSymbolTable& symbTable
			, const string& nodeName, const string& startEvent, const int type);
		bool IsChangeSymbolTable(const int itprId);
		bool IsChangeSymbolTable(const sItpr &itpr);
		bool SendSpawnInterpreterInfo(const int itprId, const int parentVmId
			, const int vmId, const string &nodeName);
		bool SendSyncVMRegister(const int itprId);
		bool SendSyncInstruction(const int itprId);
		bool SendSyncSymbolTable(const int itprId, const int vmId);
		bool SendSyncData(const int itprId, const int vmId);
		bool SendSyncVariable(const int itprId, const int vmId
			, const script::cSymbolTable &symbolTable
			, const string& varName, const script::sVariable &var);
		bool SendSyncVMInstruction(const int itprId, script::cVirtualMachine *vm);

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
		virtual bool ReqVariableInfo(remotedbg2::ReqVariableInfo_Packet& packet) override;
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
