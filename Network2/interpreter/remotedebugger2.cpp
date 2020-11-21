
#include "stdafx.h"
#include "remotedebugger2.h"

using namespace network2;


cRemoteDebugger2::cRemoteDebugger2()
	: m_syncTime(0.f)
	, m_state(eState::Stop)
	, m_debugger(&m_interpreter)
{
	ZeroMemory(m_insts, sizeof(m_insts));
	ZeroMemory(m_cmps, sizeof(m_cmps));	
}

cRemoteDebugger2::~cRemoteDebugger2()
{
	Clear();
}


// initialize RemoteDebugger Host Mode
// remotedebugger running webclient, interpreter
// connect webserver to communicate remote debugger
// receive script data from remote debugger and then run interpreter
bool cRemoteDebugger2::InitHost(const string &url
	, const int port
	, script::iFunctionCallback *callback //= nullptr
	, void *arg //= nullptr
)
{
	Clear();

	m_mode = eDebugMode::Host;
	m_url = url;
	m_port = port;

	m_interpreter.Init(callback, arg);

	m_timer.Create();
	m_client.AddProtocolHandler(this);
	m_client.RegisterProtocol(&m_protocol);

	if (!m_netController.StartWebClient(&m_client, url, port))
	{
		dbg::Logc(2, "Error WebClient Connection url:%s, port:%d\n", url.c_str(), port);
		return false;
	}
	return true;
}


// initialize RemoteDebugger Remote Mode
// not implements
bool cRemoteDebugger2::InitRemote(const Str16 &ip, const int port)
{
	return true;
}

// load intermediate code
bool cRemoteDebugger2::LoadIntermediateCode(const StrPath &fileName)
{
	RETV(m_state != eState::Stop, false);
	const bool result = m_debugger.LoadIntermediateCode(fileName);
	if (result)
		m_state = eState::Run;

	return result;
}

// process webclient, interpreter
bool cRemoteDebugger2::Process()
{
	const float dt = (float)m_timer.GetDeltaSeconds();
	m_netController.Process(dt);
	m_debugger.Process(dt);

	if (eState::Run == m_state)
	{
		m_syncTime += dt;

		// check change instruction
		set<uint> vms;
		for (uint i = 0; i < m_interpreter.m_vms.size(); ++i)
		{
			script::cVirtualMachine *vm = m_interpreter.m_vms[i];
			if (m_insts[i] != vm->m_reg.idx)
			{
				m_insts[i] = vm->m_reg.idx;
				m_cmps[i] = vm->m_reg.cmp;
				vms.insert(i); // change instruction vm
			}
		}

		// sync register?
		if (m_syncTime > 5.0f)
		{
			m_syncTime = 0.f;
			SendSyncVMRegister();
		}

		// sync instruction?
		if (!vms.empty())
		{
			for (auto idx : vms)
			{
				m_protocol.SyncVMInstruction(network2::SERVER_NETID
					, true, 0, m_insts[idx], m_cmps[idx]);
			}
		}
	}

	return m_client.IsFailConnection() ? false : true;
}


// start interpreter
bool cRemoteDebugger2::Run()
{
	RETV(m_state != eState::Run, true);
	m_debugger.Run();
	return true;
}


// stop interpreter
bool cRemoteDebugger2::Stop()
{
	m_state = eState::Stop;
	m_debugger.Terminate();
	return true;
}


// onestep interpreter if debug mode
bool cRemoteDebugger2::OneStep()
{
	RETV(m_state != eState::Run, true);
	m_debugger.OneStep();
	return true;
}


// break(pause) interpreter
bool cRemoteDebugger2::Break()
{
	RETV(m_state != eState::Run, true);
	m_debugger.Break();
	return true;
}


// clear interpreter
bool cRemoteDebugger2::Terminate()
{
	m_state = eState::Stop;
	m_debugger.Terminate();
	return true;
}


// is run interpreter?
bool cRemoteDebugger2::IsRun()
{
	return (m_state == eState::Run) && m_debugger.IsRun();
}


// sync vm register info
bool cRemoteDebugger2::SendSyncVMRegister()
{
	for (auto &vm : m_interpreter.m_vms)
	{
		m_protocol.SyncVMRegister(network2::SERVER_NETID, true, 0, 0, vm->m_reg);
		break; // now only one virtual machine sync
	}
	return true;
}


// upload visualprogram data protocol handler
bool cRemoteDebugger2::UploadVProgFile(remotedbg2::UploadVProgFile_Packet &packet) 
{ 
	// nothing
	return true; 
}


// request intermeidate code protocol handler
bool cRemoteDebugger2::ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet)
{
	const int result = m_interpreter.m_code.IsLoaded() ? 1 : 0;
	m_protocol.AckIntermediateCode(network2::SERVER_NETID, true, result, m_interpreter.m_code);
	return true;
}


// request interpreter run protocol handler
bool cRemoteDebugger2::ReqRun(remotedbg2::ReqRun_Packet &packet)
{
	Run();
	m_protocol.AckRun(network2::SERVER_NETID, false, 1);
	SendSyncVMRegister();
	return true;
}


// request one stp debugging protocol handler
bool cRemoteDebugger2::ReqOneStep(remotedbg2::ReqOneStep_Packet &packet)
{
	OneStep();
	m_protocol.AckOneStep(network2::SERVER_NETID, false, 1);
	return true;
}


// request interprter break to debugging protocol handler
bool cRemoteDebugger2::ReqBreak(remotedbg2::ReqBreak_Packet &packet)
{
	Break();
	m_protocol.AckBreak(network2::SERVER_NETID, false, 1);
	return true;
}


// request interpreter stop protocol handler
bool cRemoteDebugger2::ReqStop(remotedbg2::ReqStop_Packet &packet)
{
	Stop();
	m_protocol.AckStop(network2::SERVER_NETID, false, 1);
	return true;
}


// request interpreter input event protocol handler
bool cRemoteDebugger2::ReqInput(remotedbg2::ReqInput_Packet &packet)
{
	script::cEvent evt(packet.eventName);
	m_interpreter.PushEvent(evt);
	m_protocol.AckInput(network2::SERVER_NETID, false, 1);
	return true;
}


// clear all data
void cRemoteDebugger2::Clear()
{
	m_state = eState::Stop;
	m_client.Close();
	m_netController.Clear();
}

