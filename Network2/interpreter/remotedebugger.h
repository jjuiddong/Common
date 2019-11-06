//
// 2019-11-06, jjuiddong
// common::script::interpreter remote debugger
//		- debugging with tcp/ip networking
//
#pragma once

#include "Src/remotedbg_Protocol.h"
#include "Src/remotedbg_ProtocolHandler.h"


namespace network2
{

	class cRemoteDebugger : public remotedbg::s2c_ProtocolHandler
						, public remotedbg::c2s_ProtocolHandler
	{
	public:
		enum class eDebugMode { None, Host, Remote};

		cRemoteDebugger();
		virtual ~cRemoteDebugger();

		bool Init(const eDebugMode mode
			, const Str16 &ip
			, const int port
			, common::script::cDebugger *debugger
		);
		bool Process(const float deltaSeconds);
		bool Start();
		bool Stop();
		bool OneStep();
		bool DebugRun();
		bool Break();
		bool Terminate();
		bool IsRun();
		void Clear();


	protected:
		bool InternalProcess(const float deltaSeconds);
		bool HostProcess(const float deltaSeconds);
		bool RemoteProcess(const float deltaSeconds);

		// Remote Debugging Protocol Handler (Host)
		virtual bool ReqOneStep(remotedbg::ReqOneStep_Packet &packet) override;
		virtual bool ReqDebugRun(remotedbg::ReqDebugRun_Packet &packet) override;
		virtual bool ReqBreak(remotedbg::ReqBreak_Packet &packet) override;
		virtual bool ReqTerminate(remotedbg::ReqTerminate_Packet &packet) override;

		// Remote Debugging Protocol Handler (Remote Client)
		virtual bool UpdateInformation(remotedbg::UpdateInformation_Packet &packet) override;
		virtual bool UpdateState(remotedbg::UpdateState_Packet &packet) override;
		virtual bool UpdateSymbolTable(remotedbg::UpdateSymbolTable_Packet &packet) override;


	public:
		enum class eState { Stop, Run };

		eState m_state;
		eDebugMode m_mode;
		common::script::cDebugger *m_debugger; // reference
											// synchronize debugger

		map<StrId, int> m_checkMap; // key:vmName, value:state id
									// work for host debugger
									// check change interpreter to send state info

		map<StrId, common::cCircularQueue<int, 100>> m_vmDbgs; // debug info queue
									// from network recv host remote debugger

		// network
		Str16 m_ip;
		int m_port;
		float m_incTime;
		network2::cTcpClient m_dbgClient;
		network2::cTcpServer m_dbgServer;
		remotedbg::s2c_Protocol m_hostProtocol;
		remotedbg::c2s_Protocol m_remoteProtocol;
		network2::cNetController m_netController;
	};

}
