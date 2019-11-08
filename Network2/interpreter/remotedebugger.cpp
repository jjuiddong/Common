
#include "stdafx.h"
#include "remotedebugger.h"

using namespace network2;


cRemoteDebugger::cRemoteDebugger()
	: m_mode(eDebugMode::None)
	, m_debugger(nullptr)
	, m_incTime(0)
	, m_state(eState::Stop)
{
}

cRemoteDebugger::~cRemoteDebugger()
{
	Clear();
}


bool cRemoteDebugger::Init(const eDebugMode mode
	, const Str16 &ip
	, const int port
	, common::script::cDebugger *debugger
)
{
	Clear();

	m_mode = mode;
	m_ip = ip;
	m_port = port;
	m_debugger = debugger;
	return true;
}


bool cRemoteDebugger::Process(const float deltaSeconds)
{
	m_netController.Process(deltaSeconds);
	InternalProcess(deltaSeconds);
	return true;
}


bool cRemoteDebugger::Start()
{
	Stop();

	bool result = false;
	switch (m_mode)
	{
	case eDebugMode::Remote:
		m_dbgClient.AddProtocolHandler(this);
		m_dbgClient.RegisterProtocol(&m_remoteProtocol);
		result = m_netController.StartTcpClient(&m_dbgClient, m_ip, m_port);
		break;

	case eDebugMode::Host:
		m_dbgServer.AddProtocolHandler(this);
		m_dbgServer.RegisterProtocol(&m_hostProtocol);
		result = m_netController.StartTcpServer(&m_dbgServer, m_port);
		break;

	default:
		assert(0);
		return false;
	}

	if (result)
		m_state = eState::Run;

	return result;
}


bool cRemoteDebugger::Stop()
{
	switch (m_mode)
	{
	case eDebugMode::Remote: m_dbgClient.Close(); break;
	case eDebugMode::Host: m_dbgServer.Close(); break;
	default: assert(0); return false;
	}
	m_netController.Clear();
	m_state = eState::Stop;
	return true;
}


// one step debugging for remote client mode
bool cRemoteDebugger::OneStep()
{
	RETV(eDebugMode::Remote != m_mode, false);
	RETV(!m_dbgClient.IsConnect(), false);

	m_remoteProtocol.ReqOneStep(network2::SERVER_NETID);
	return true;
}


bool cRemoteDebugger::DebugRun()
{
	RETV(eDebugMode::Remote != m_mode, false);
	RETV(!m_dbgClient.IsConnect(), false);

	m_remoteProtocol.ReqDebugRun(network2::SERVER_NETID);
	return true;
}


bool cRemoteDebugger::Break()
{
	RETV(eDebugMode::Remote != m_mode, false);
	RETV(!m_dbgClient.IsConnect(), false);

	m_remoteProtocol.ReqBreak(network2::SERVER_NETID);
	return true;
}


bool cRemoteDebugger::Terminate()
{
	RETV(eDebugMode::Remote != m_mode, false);
	RETV(!m_dbgClient.IsConnect(), false);

	m_remoteProtocol.ReqTerminate(network2::SERVER_NETID);
	return true;
}


bool cRemoteDebugger::IsRun()
{
	return m_state == eState::Run;
}


// process remote debugger
bool cRemoteDebugger::InternalProcess(const float deltaSeconds)
{
	RETV(!m_debugger, false);
	RETV(!m_debugger->IsLoad(), false);

	switch (m_mode)
	{
	case eDebugMode::Host: HostProcess(deltaSeconds); break;
	case eDebugMode::Remote: RemoteProcess(deltaSeconds); break;
	default: assert(0); break;
	}

	return true;
}


// host remote debugger process
bool cRemoteDebugger::HostProcess(const float deltaSeconds)
{
	common::script::cInterpreter *interpreter = m_debugger->m_interpreter;

	// check change interpreter state
	set<StrId> sendVms;
	for (auto &vm : interpreter->m_vms)
	{
		auto it = m_checkMap.find(vm->m_name);
		if (m_checkMap.end() == it)
		{
			m_checkMap[vm->m_name] = -1;
			sendVms.insert(vm->m_name);
		}
		else
		{
			const bool isSnd = (vm->m_reg.idx != it->second);
			if (isSnd)
			{
				m_checkMap[vm->m_name] = vm->m_reg.idx;
				sendVms.insert(vm->m_name);
			}
		}
	}

	if (sendVms.empty())
		return true;

	// send debugging information
	for (auto &name : sendVms)
	{
		auto it = std::find_if(interpreter->m_vms.begin(), interpreter->m_vms.end()
			, [&](const auto &vm) { return vm->m_name == name; });
		if (interpreter->m_vms.end() == it)
			continue;

		common::script::cVirtualMachine *vm = *it;
		m_hostProtocol.UpdateState(ALL_NETID, vm->m_name.c_str()
			, m_checkMap[name]);
	}

	return true;
}



// remote debugger client process
bool cRemoteDebugger::RemoteProcess(const float deltaSeconds)
{
	// nothing~
	return true;
}


void cRemoteDebugger::Clear()
{
	if (eDebugMode::None != m_mode)
		Stop();
	m_checkMap.clear();
	m_dbgClient.Close();
	m_dbgServer.Close();
}


// if remote debugger connection, send debug information
void cRemoteDebugger::AddSession(cSession &session)
{
	RET(!m_debugger);
	RET(!m_debugger->IsLoad());

	common::script::cInterpreter *interpreter = m_debugger->m_interpreter;
	RET(!interpreter->IsDebug());
	RET(interpreter->m_vms.empty());

	const StrPath &fileName = interpreter->m_fileName;
	common::script::cVirtualMachine *vm = interpreter->m_vms.front();
	const StrId &vmName = vm->m_name;

	m_hostProtocol.UpdateInformation(network2::ALL_NETID
		, fileName.GetFileName(), vmName.c_str(), vm->m_reg.idx);
}


//-------------------------------------------------------------------
// RemoteDebugger Host Side Protocol Handler
//-------------------------------------------------------------------
bool cRemoteDebugger::ReqOneStep(remotedbg::ReqOneStep_Packet &packet) 
{
	RETV(eDebugMode::Host != m_mode, true);
	RETV(!m_debugger, true);
	
	m_debugger->OneStep();
	return true; 
}


bool cRemoteDebugger::ReqDebugRun(remotedbg::ReqDebugRun_Packet &packet) 
{ 
	RETV(eDebugMode::Host != m_mode, true);
	RETV(!m_debugger, true);

	m_debugger->Run();
	return true; 
}


bool cRemoteDebugger::ReqBreak(remotedbg::ReqBreak_Packet &packet) 
{ 
	RETV(eDebugMode::Host != m_mode, true);
	RETV(!m_debugger, true);

	m_debugger->Break();
	return true; 
}


bool cRemoteDebugger::ReqTerminate(remotedbg::ReqTerminate_Packet &packet) 
{ 
	RETV(eDebugMode::Host != m_mode, true);
	RETV(!m_debugger, true);

	m_debugger->Terminate();
	return true; 
}


//-------------------------------------------------------------------
// RemoteDebugger Remote Client Side Protocol Handler
//-------------------------------------------------------------------
bool cRemoteDebugger::UpdateInformation(remotedbg::UpdateInformation_Packet &packet) 
{
	RETV(eDebugMode::Remote != m_mode, true);

	return true; 
}


bool cRemoteDebugger::UpdateState(remotedbg::UpdateState_Packet &packet) 
{
	RETV(eDebugMode::Remote != m_mode, true);

	m_vmDbgs[packet.vmName].push(packet.instIndex);
	return true; 
}


bool cRemoteDebugger::UpdateSymbolTable(remotedbg::UpdateSymbolTable_Packet &packet) 
{
	RETV(eDebugMode::Remote != m_mode, true);

	return true; 
}
