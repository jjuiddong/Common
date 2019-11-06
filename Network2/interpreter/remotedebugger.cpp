
#include "stdafx.h"
#include "remotedebugger.h"

using namespace network2;


cRemoteDebugger::cRemoteDebugger()
	: m_type(eDebugType::None)
	, m_debugger(nullptr)
	, m_incTime(0)
	, m_state(eState::Stop)
{
}

cRemoteDebugger::~cRemoteDebugger()
{
	Clear();
}


bool cRemoteDebugger::Init(const eDebugType type
	, const Str16 &ip
	, const int port
	, common::script::cDebugger *debugger
)
{
	Clear();

	m_type = type;
	m_ip = ip;
	m_port = port;
	m_debugger = debugger;
	return true;
}


bool cRemoteDebugger::Update(const float deltaSeconds)
{
	m_netController.Process(deltaSeconds);
	Process(deltaSeconds);
	return true;
}


bool cRemoteDebugger::Start()
{
	Stop();

	bool result = false;
	switch (m_type)
	{
	case eDebugType::Remote:
		m_dbgClient.AddProtocolHandler(this);
		m_dbgClient.RegisterProtocol(&m_remotedbgProtocol);
		result = m_netController.StartTcpClient(&m_dbgClient, m_ip, m_port);
		break;

	case eDebugType::Host:
		m_dbgServer.AddProtocolHandler(this);
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
	switch (m_type)
	{
	case eDebugType::Remote: m_dbgClient.Close(); break;
	case eDebugType::Host: m_dbgServer.Close(); break;
	default: assert(0); return false;
	}
	m_netController.Clear();
	m_state = eState::Stop;
	return true;
}


bool cRemoteDebugger::IsRun()
{
	return m_state == eState::Run;
}


// process remote debugger
bool cRemoteDebugger::Process(const float deltaSeconds)
{
	RETV(!m_debugger, false);
	RETV(!m_debugger->IsLoad(), false);

	switch (m_type)
	{
	case eDebugType::Host: HostProcess(deltaSeconds); break;
	case eDebugType::Remote: RemoteProcess(deltaSeconds); break;
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
		m_remotedbgProtocol.UpdateState(ALL_NETID, vm->m_name.c_str()
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


bool cRemoteDebugger::ReqOneStep(remotedbg::ReqOneStep_Packet &packet) 
{
	RETV(eDebugType::Host != m_type, true);

	return true; 
}


bool cRemoteDebugger::ReqDebugRun(remotedbg::ReqDebugRun_Packet &packet) 
{ 
	RETV(eDebugType::Host != m_type, true);

	return true; 
}


bool cRemoteDebugger::ReqBreak(remotedbg::ReqBreak_Packet &packet) 
{ 
	RETV(eDebugType::Host != m_type, true);

	return true; 
}


bool cRemoteDebugger::ReqTerminate(remotedbg::ReqTerminate_Packet &packet) 
{ 
	RETV(eDebugType::Host != m_type, true);

	return true; 
}


bool cRemoteDebugger::UpdateInformation(remotedbg::UpdateInformation_Packet &packet) 
{
	RETV(eDebugType::Remote != m_type, true);

	return true; 
}


bool cRemoteDebugger::UpdateState(remotedbg::UpdateState_Packet &packet) 
{
	RETV(eDebugType::Remote != m_type, true);

	return true; 
}


bool cRemoteDebugger::UpdateSymbolTable(remotedbg::UpdateSymbolTable_Packet &packet) 
{
	RETV(eDebugType::Remote != m_type, true);

	return true; 
}


void cRemoteDebugger::Clear()
{
	if (eDebugType::None != m_type)
		Stop();
	m_checkMap.clear();
	m_dbgClient.Close();
	m_dbgServer.Close();
}
