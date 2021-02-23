
#include "stdafx.h"
#include "remotedebugger2.h"

using namespace network2;

const float TIME_SYNC_INSTRUCTION = 0.5f;
const float TIME_SYNC_REGISTER = 5.0f;
const float TIME_SYNC_SYMBOL = 5.0f;


cRemoteDebugger2::cRemoteDebugger2()
	: m_regSyncTime(0.f)
	, m_instSyncTime(0.f)
	, m_symbSyncTime(0.f)
	, m_state(eState::Stop)
	, m_debugger(&m_interpreter)
	, m_isChangeInstruction(false)
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
bool cRemoteDebugger2::InitHost(cNetController &netController
	, const string &url
	, script::iFunctionCallback *callback //= nullptr
	, void *arg //= nullptr
)
{
	Clear();

	m_mode = eDebugMode::Host;
	m_url = url;

	m_interpreter.Init(callback, arg);

	m_client.AddProtocolHandler(this);
	m_client.RegisterProtocol(&m_protocol);

	if (!netController.StartWebClient(&m_client, url))
	{
		dbg::Logc(2, "Error WebClient Connection url:%s \n", url.c_str());
		return false;
	}
	return true;
}


// initialize RemoteDebugger Remote Mode
// not implements
bool cRemoteDebugger2::InitRemote(cNetController &netController
	, const Str16 &ip, const int port)
{
	return true;
}


// load intermediate code
bool cRemoteDebugger2::LoadIntermediateCode(const StrPath &fileName)
{
	RETV(m_state != eState::Stop, false);
	const bool result = m_debugger.LoadIntermediateCode(fileName);
	//if (result)
	//	m_state = eState::Run;
	return result;
}


// process webclient, interpreter
bool cRemoteDebugger2::Process(const float deltaSeconds)
{
	m_debugger.Process(deltaSeconds);

	// sync instruction, register, syboltable
	if (eState::Run == m_state)
	{
		m_regSyncTime += deltaSeconds;
		m_instSyncTime += deltaSeconds;
		m_symbSyncTime += deltaSeconds;

		// check change instruction
		for (uint i = 0; i < m_interpreter.m_vms.size(); ++i)
		{
			script::cVirtualMachine *vm = m_interpreter.m_vms[i];
			if (m_insts[i].empty() 
				|| ((m_insts[i].size() < 100)
					&& (m_insts[i].back() != vm->m_reg.idx)))
			{
				m_insts[i].push_back(vm->m_reg.idx);
				m_cmps[i].push_back(vm->m_reg.cmp);
				m_isChangeInstruction = true;

				// sync delay instruction
				if (script::eCommand::delay ==
					vm->m_code.m_codes[vm->m_reg.exeIdx].cmd)
				{
					// sync instruction, register
					m_instSyncTime = TIME_SYNC_INSTRUCTION + 1.f;
					m_regSyncTime = TIME_SYNC_REGISTER + 1.f;
				}
			}
		}

		// sync register?
		if (m_regSyncTime > TIME_SYNC_REGISTER)
		{
			m_regSyncTime = 0.f;
			SendSyncVMRegister();
		}

		// sync instruction?
		if (m_isChangeInstruction && (m_instSyncTime > TIME_SYNC_INSTRUCTION))
		{
			m_instSyncTime = 0.f;
			m_isChangeInstruction = false;
			for (uint i = 0; i < 10; ++i)
			{
				if (m_insts[i].empty())
					continue;
				
				m_protocol.SyncVMInstruction(network2::SERVER_NETID
					, true, 0, m_insts[i], m_cmps[i]);

				// clear and setup last data
				const uint index = m_insts[i].back();
				const bool cmp = m_cmps[i].back();
				m_insts[i].clear();
				m_cmps[i].clear();
				m_insts[i].push_back(index); // last data
				m_cmps[i].push_back(cmp); // last data
			}
		}

		// sync symboltable
		if (m_symbSyncTime > TIME_SYNC_SYMBOL) {
			m_symbSyncTime = 0.f;

			uint symbolCount = 0;
			for (uint i = 0; i < m_interpreter.m_vms.size(); ++i)
			{
				vector<script::sSyncSymbol> symbols;
				script::cVirtualMachine *vm = m_interpreter.m_vms[i];
				for (auto &kv1 : vm->m_symbTable.m_vars)
				{
					// tricky code, packet buffer overflow
					if (symbolCount >= 15)
						break;

					const string &scope = kv1.first;
					for (auto &kv2 : kv1.second)
					{
						// tricky code, packet buffer overflow
						if (symbolCount >= 15)
							break;
						symbolCount++;

						const string &name = kv2.first;
						const script::sVariable &var = kv2.second;
						symbols.push_back(script::sSyncSymbol(&scope, &name, &var.var));
					}
				}

				if (!symbols.empty())
				{
					m_protocol.SyncVMSymbolTable(network2::SERVER_NETID, true
						, i, 0, symbolCount, symbols);
				}
			}
		}
	}

	return m_client.IsFailConnection() ? false : true;
}


// push interpreter event, wraping function
bool cRemoteDebugger2::PushEvent(const common::script::cEvent &evt)
{
	return m_interpreter.PushEvent(evt);	
}


// start interpreter
bool cRemoteDebugger2::Run()
{
	RETV(m_state != eState::Stop, true);
	if (m_debugger.Run())
	{
		m_state = eState::Run;
		return true;
	}
	return false;
}


// start interpreter with one step debugging
bool cRemoteDebugger2::StepRun()
{
	RETV(m_state != eState::Stop, true);
	if (m_debugger.StepRun())
	{
		m_state = eState::Run;
		return true;
	}
	return false;
}


// stop interpreter
bool cRemoteDebugger2::Stop()
{
	m_state = eState::Stop;
	return m_debugger.Stop();
}


// resume run interpreter
bool cRemoteDebugger2::Resume()
{
	RETV(m_state != eState::Run, false);
	return m_debugger.Resume();
}


// onestep interpreter if debug mode
bool cRemoteDebugger2::OneStep()
{
	RETV(m_state != eState::Run, false);
	return m_debugger.OneStep();
}


// break(pause) interpreter
bool cRemoteDebugger2::Break()
{
	RETV(m_state != eState::Run, false);
	return m_debugger.Break();
}


// is run interpreter?
bool cRemoteDebugger2::IsRun()
{
	return (m_state == eState::Run) && m_debugger.IsRun();
}

// is debugging interpreter?
bool cRemoteDebugger2::IsDebug()
{
	return (m_state == eState::Run) && m_debugger.IsDebug();
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


// welcome packet from remote server
bool cRemoteDebugger2::Welcome(remotedbg2::Welcome_Packet &packet)
{ 
	dbg::Logc(0, "Success Connection to Remote Debugger Server\n");
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
	// tricky code, marshalling intermedatecode to byte stream
	const uint BUFFER_SIZE = 1024 * 50;
	BYTE *buff = new BYTE[BUFFER_SIZE];
	cPacket marsh(m_client.GetPacketHeader());
	marsh.m_data = buff;
	marsh.m_bufferSize = BUFFER_SIZE;
	marsh.m_writeIdx = 0;
	network2::marshalling::operator<<(marsh, m_interpreter.m_code);
	
	// send split
	const uint bufferSize = (uint)marsh.m_writeIdx;
	if (bufferSize == 0)
	{
		// no intermediate code, fail!
		m_protocol.AckIntermediateCode(network2::SERVER_NETID, true, 0, 0, 0, {});
	}
	else
	{
		const uint CHUNK_SIZE = network2::DEFAULT_PACKETSIZE -
			(marsh.GetHeaderSize() + 16); // 4 * 4byte = result,count,index,buffsize
		const uint totalCount = ((bufferSize % CHUNK_SIZE) == 0) ?
			bufferSize / CHUNK_SIZE : bufferSize / CHUNK_SIZE + 1;

		uint index = 0;
		uint cursor = 0;
		while (cursor < bufferSize) 
		{
			const uint size = std::min(bufferSize - cursor, CHUNK_SIZE);
			vector<BYTE> data(size);
			memcpy_s(&data[0], size, &marsh.m_data[cursor], size);
			m_protocol.AckIntermediateCode(network2::SERVER_NETID, true
				, 1, totalCount, index, data);

			cursor += size;
			++index;
		}
	}

	SAFE_DELETEA(buff);
	return true;
}


// request interpreter run protocol handler
bool cRemoteDebugger2::ReqRun(remotedbg2::ReqRun_Packet &packet)
{
	if (packet.runType == "Run")
		Run();
	else if (packet.runType == "DebugRun")
		Run();
	else if (packet.runType == "StepRun")
		StepRun();

	const int result = m_debugger.IsRun() ? 1 : 0;
	m_protocol.AckRun(network2::SERVER_NETID, false, result);
	SendSyncVMRegister();
	return true;
}


// request one stp debugging protocol handler
bool cRemoteDebugger2::ReqOneStep(remotedbg2::ReqOneStep_Packet &packet)
{
	OneStep();

	// every step debugging, synchronize instruction, register, symboltable information
	m_regSyncTime = TIME_SYNC_REGISTER + 1.0f;
	m_instSyncTime = TIME_SYNC_INSTRUCTION + 1.0f;
	m_symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;

	m_protocol.AckOneStep(network2::SERVER_NETID, false, 1);
	return true;
}

// request resume debugging run protocol handler
bool cRemoteDebugger2::ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet)
{
	Resume();
	m_protocol.AckResumeRun(network2::SERVER_NETID, false, 1);
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
}

