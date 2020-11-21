
#include "stdafx.h"
#include "remotedebugger2.h"

using namespace network2;


cRemoteDebugger2::cRemoteDebugger2()
	: m_syncTime(0.f)
	, m_state(eState::Stop)
{
}

cRemoteDebugger2::~cRemoteDebugger2()
{
	Clear();
}


// initialize RemoteDebugger Host Mode
// remotedebugger running webclient, interpreter
// connect webserver to communicate remote debugger
// receive script data from remote debugger and then run interpreter
bool cRemoteDebugger2::InitHost(const string &url, const int port)
{
	Clear();

	m_mode = eDebugMode::Host;
	m_url = url;
	m_port = port;

	m_timer.Create();
	m_client.AddProtocolHandler(this);
	m_client.RegisterProtocol(&m_protocol);

	if (!m_netController.StartWebClient(&m_client, url, port))
	{
		dbg::Logc(2, "Error WebClient Connection url:%s, port:%d\n", url.c_str(), port);
		return false;
	}

	m_state = eState::Run;
	return true;
}


// initialize RemoteDebugger Remote Mode
// not implements
bool cRemoteDebugger2::InitRemote(const Str16 &ip, const int port)
{
	return true;
}


// process webclient, interpreter
bool cRemoteDebugger2::Process()
{
	const float dt = (float)m_timer.GetDeltaSeconds();
	m_netController.Process(dt);
	m_interpreter.Process(dt);

	m_syncTime += dt;
	if (m_syncTime > 5.0f)
	{
		m_syncTime = 0.f;

		// sync register
		for (auto &vm : m_interpreter.m_vms)
		{
			m_protocol.SyncVMRegister(network2::SERVER_NETID, true, 0, 0, vm->m_reg);
			break; // now only one interpreter sync
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
	return true;
}


// request one stp debugging protocol handler
bool cRemoteDebugger2::ReqOneStep(remotedbg2::ReqOneStep_Packet &packet)
{
	OneStep();
	return true;
}


// request interprter break to debugging protocol handler
bool cRemoteDebugger2::ReqBreak(remotedbg2::ReqBreak_Packet &packet)
{
	Break();
	return true;
}


// request interpreter stop protocol handler
bool cRemoteDebugger2::ReqStop(remotedbg2::ReqStop_Packet &packet)
{
	Terminate();
	return true;
}


// request interpreter input event protocol handler
bool cRemoteDebugger2::ReqInput(remotedbg2::ReqInput_Packet &packet)
{
	return true;
}


// clear all data
void cRemoteDebugger2::Clear()
{
	m_state = eState::Stop;
	m_client.Close();
	m_netController.Clear();
}

