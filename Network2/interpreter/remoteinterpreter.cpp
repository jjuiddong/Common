
#include "stdafx.h"
#include "remoteinterpreter.h"

using namespace network2;

namespace
{
	const float TIME_SYNC_INSTRUCTION = 0.5f;
	const float TIME_SYNC_REGISTER = 5.0f;
	const float TIME_SYNC_SYMBOL = 3.0f;
}

//----------------------------------------------------------------------------------
// Send Intermediate Code Task
bool SendIntermediateCode(remotedbg2::h2r_Protocol &protocol
	, const netid recvId, const int itprId, const script::cIntermediateCode &icode);

class cSendICodeTask : public common::cTask
{
public:
	cSendICodeTask(cRemoteInterpreter *rmtItpr, const int itprId, const netid rcvId)
		: cTask(1, "cSendICodeTask")
		, m_rmtItpr(rmtItpr), m_itprId(itprId), m_rcvId(rcvId) {
	}
	virtual eRunResult Run(const double deltaSeconds)
	{
		if (m_rmtItpr->m_interpreters.size() <= (uint)m_itprId) {
			// fail, iterpreter id invalid
			m_rmtItpr->m_protocol.AckIntermediateCode(m_rcvId, true, m_itprId
				, 0, 0, 0, 0, {});
			--m_rmtItpr->m_multiThreading;
			return eRunResult::End;
		}
		script::cInterpreter *interpreter = 
			m_rmtItpr->m_interpreters[m_itprId].interpreter;
		SendIntermediateCode(m_rmtItpr->m_protocol, m_rcvId, m_itprId
			, interpreter->m_code);
		--m_rmtItpr->m_multiThreading;
		return eRunResult::End;
	}

public:
	cRemoteInterpreter *m_rmtItpr; // reference
	int m_itprId;
	netid m_rcvId;
};


//----------------------------------------------------------------------------------
// cRemoteInterpreter
cRemoteInterpreter::cRemoteInterpreter(
	const StrId &name
	, const int logId //= -1
)
	: m_server(new network2::cWebSessionFactory(), name, logId)
	, m_callback(nullptr)
	, m_arg(nullptr)
	, m_threads(nullptr)
	, m_multiThreading(0)
	, m_isThreadMode(true)
{
}

cRemoteInterpreter::~cRemoteInterpreter()
{
	Clear();
}


// reuse remote interpreter
bool cRemoteInterpreter::Reuse(const StrId &name, const int logId)
{
	m_server.m_name = name;
	m_server.m_id = common::GenerateId(); // new netid
	m_server.SetLogId(logId);
	network2::LogSession(logId, m_server);
	m_server.m_sendQueue.ClearBuffer();
	m_server.m_recvQueue.ClearBuffer();

	m_callback = nullptr;
	m_arg = nullptr;
	m_threads = nullptr;
	m_multiThreading = 0;
	return true;
}


// initialize RemoteInterpreter
// remote interpreter run webserver
// receive script data from remote debugger and then run interpreter
bool cRemoteInterpreter::Init(cNetController &netController
	, const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //=true
	, script::iFunctionCallback *callback //= nullptr
	, void *arg //= nullptr
)
{
	Clear();

	m_callback = callback;
	m_bindPort = bindPort;
	m_arg = arg;
	m_isThreadMode = isThreadMode;

	m_server.AddProtocolHandler(this);
	m_server.RegisterProtocol(&m_protocol);

	if (!netController.StartWebServer(&m_server, bindPort, packetSize
		, maxPacketCount, sleepMillis, isThreadMode))
	{
		dbg::Logc(2, "Error Start WebServer port:%d \n", bindPort);
		return false;
	}

	return true;
}


// load intermediate code
// fileName: intermediate code filename
bool cRemoteInterpreter::LoadIntermediateCode(const StrPath &fileName)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init(m_callback, m_arg ? m_arg : interpreter);

	if (!interpreter->LoadIntermediateCode(fileName))
	{
		delete interpreter;
		return false;
	}

	sItpr itpr;
	itpr.name = interpreter->m_fileName.c_str();
	itpr.state = eState::Stop;
	itpr.interpreter = interpreter;
	itpr.isChangeInstruction = false;
	itpr.regSyncTime = 0.f;
	itpr.instSyncTime = 0.f;
	itpr.symbSyncTime = 0.f;

	m_interpreters.push_back(itpr);
	return true;
}


// load intermediate code 
// fileNames: intermediate code filename array
bool cRemoteInterpreter::LoadIntermediateCode(const vector<StrPath> &fileNames)
{
	ClearInterpreters();
	m_interpreters.reserve(fileNames.size());

	bool isSuccess = true;
	for (auto &fileName : fileNames)
	{
		if (!LoadIntermediateCode(fileNames))
		{
			isSuccess = false;
			break;
		}
	}
	if (!isSuccess)
		ClearInterpreters();
	return isSuccess;
}


// load intermediate code
bool cRemoteInterpreter::LoadIntermediateCode(
	const common::script::cIntermediateCode &icode)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init(m_callback, m_arg ? m_arg : interpreter);

	if (!interpreter->LoadIntermediateCode(icode))
	{
		delete interpreter;
		return false;
	}

	sItpr itpr;
	itpr.name = interpreter->m_fileName.c_str();
	itpr.state = eState::Stop;
	itpr.interpreter = interpreter;
	itpr.isChangeInstruction = false;
	itpr.regSyncTime = 0.f;
	itpr.instSyncTime = 0.f;
	itpr.symbSyncTime = 0.f;

	m_interpreters.push_back(itpr);
	return true;
}


// process webclient, interpreter
bool cRemoteInterpreter::Process(const float deltaSeconds)
{
	// sync instruction, register, syboltable
	// check change instruction
	for (uint k=0; k < m_interpreters.size(); ++k)
	{
		const int itprId = (int)k;
		sItpr &itpr = m_interpreters[itprId];
		itpr.interpreter->Process(deltaSeconds);

		if (itpr.state != eState::Run)
			continue;

		itpr.regSyncTime += deltaSeconds;
		itpr.instSyncTime += deltaSeconds;
		itpr.symbSyncTime += deltaSeconds;

		script::cInterpreter *interpreter = itpr.interpreter;
		for (uint i = 0; i < interpreter->m_vms.size(); ++i)
		{
			script::cVirtualMachine *vm = interpreter->m_vms[i];

			// sync delay instruction (check next instruction is delay node?)
			// 'vm->m_reg.idx' is next execute instruction code index
			// ldtim is previous delay command
			if (script::eCommand::ldtim ==
				vm->m_code.m_codes[vm->m_reg.idx].cmd)
			{
				// sync instruction & register
				itpr.instSyncTime = TIME_SYNC_INSTRUCTION + 1.f;
				itpr.regSyncTime = TIME_SYNC_REGISTER + 1.f;
			}
		}

		// sync register?
		if (itpr.regSyncTime > TIME_SYNC_REGISTER)
		{
			itpr.regSyncTime = 0.f;
			SendSyncVMRegister(itprId);
		}

		// sync instruction?
		if (itpr.instSyncTime > TIME_SYNC_INSTRUCTION)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			for (uint i = 0; i < interpreter->m_vms.size(); ++i)
			{
				script::cVirtualMachine *vm = interpreter->m_vms[i];
				if (vm->m_trace.empty())
					continue; // no process
				const uint size = vm->m_trace.size();
				if ((size == 2) && (vm->m_trace[0] == vm->m_trace[1]))
					continue; // no process, no changed

				itpr.instSyncTime = 0.f;
				itpr.isChangeInstruction = false; // initialize flag

				m_protocol.SyncVMInstruction(network2::ALL_NETID
					, true, itprId, i, vm->m_trace);

				vm->ClearCodeTrace(true);
			}
		}

		SendSyncSymbolTable(itprId);

		// is meet breakpoint? change step debugging mode
		if (interpreter->IsBreak())
		{
			m_protocol.AckOneStep(network2::ALL_NETID, true, itprId, 1);
		}
	}

	return m_server.IsFailConnection() ? false : true;
}


// push interpreter event, wraping function
// isUnique: no has same event, (check same event)
bool cRemoteInterpreter::PushEvent(const int itprId
	, const common::script::cEvent &evt
	, const bool isUnique //= false
)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
				interpreter->PushEvent(evt, isUnique);
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;

		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->PushEvent(evt, isUnique);
	}
	return true;
}


// start interpreter
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::Run(const int itprId)
{
	if (itprId < 0) 
	{
		for (auto &itpr : m_interpreters)
		{
			if (itpr.state != eState::Stop)
				continue;
			script::cInterpreter *interpreter = itpr.interpreter;
			interpreter->SetCodeTrace(true);
			if (interpreter->Run())
				itpr.state = eState::Run;
		}
	}
	else 
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (itpr.state != eState::Stop)
			return true;

		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->SetCodeTrace(true);
		if (interpreter->Run())
		{
			itpr.state = eState::Run;
			return true;
		}
		return false;
	}
	return true;
}


// start interpreter with debug run
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::DebugRun(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			if (eState::Stop != itpr.state)
				continue;
			script::cInterpreter *interpreter = itpr.interpreter;
			interpreter->SetCodeTrace(true);
			if (interpreter->DebugRun())
				itpr.state = eState::Run;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Stop != itpr.state)
			return true;

		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->SetCodeTrace(true);
		if (interpreter->DebugRun())
		{
			itpr.state = eState::Run;
			return true;
		}
		return false;
	}
	return true;
}


// start interpreter with one step debugging
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::StepRun(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			if (eState::Stop != itpr.state)
				continue;
			script::cInterpreter *interpreter = itpr.interpreter;
			if (interpreter->StepRun())
				itpr.state = eState::Run;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Stop != itpr.state)
			return true;

		script::cInterpreter *interpreter = itpr.interpreter;
		if (interpreter->StepRun())
		{
			itpr.state = eState::Run;
			return true;
		}
		return false;
	}
	return true;
}


// stop interpreter
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::Stop(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			interpreter->Stop();
			itpr.state = eState::Stop;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;

		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Stop();
		itpr.state = eState::Stop;
	}
	return true;
}


// resume run interpreter
// itprId: interpreter index
bool cRemoteInterpreter::Resume(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
				interpreter->Resume();
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		return interpreter->Resume();
	}
	return true;
}


// resume virtual machine
bool cRemoteInterpreter::ResumeVM(const int itprId, const string &vmName)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
				interpreter->ResumeVM(vmName);
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		return interpreter->ResumeVM(vmName);
	}
	return true;
}


// onestep interpreter if debug mode
// itprId: interpreter index
bool cRemoteInterpreter::OneStep(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
				interpreter->OneStep();
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		return interpreter->OneStep();
	}
	return true;
}


// break(pause) interpreter
// itprId: interpreter index
bool cRemoteInterpreter::Break(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
				interpreter->Break();
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Break();
	}
	return true;
}


// set break point
// itprId: interpreter index
bool cRemoteInterpreter::BreakPoint(const int itprId, const bool enable, const uint id)
{
	if (itprId < 0)
	{
		return false;
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->BreakPoint(enable, id);
	}
	return true;
}


// is run or debug run interpreter?
// itprId: interpreter index
bool cRemoteInterpreter::IsRun(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			const bool res = (eState::Run == itpr.state) && 
				(interpreter->IsRun() || interpreter->IsDebugRun());
			if (res)
				return true;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Break();
		return (eState::Run == itpr.state) && interpreter->IsRun();
	}
	return false;
}

// is debugging interpreter?
// itprId: interpreter index
bool cRemoteInterpreter::IsDebug(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			const bool res = (eState::Run == itpr.state) && interpreter->IsDebug();
			if (res)
				return true;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Break();
		return (eState::Run == itpr.state) && interpreter->IsDebug();
	}
	return false;
}


// sync vm register info
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncVMRegister(const int itprId)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;
	script::cInterpreter *interpreter = m_interpreters[itprId].interpreter;
	for (uint i=0; i < interpreter->m_vms.size(); ++i)
	{
		auto &vm = interpreter->m_vms[i];
		m_protocol.SyncVMRegister(network2::ALL_NETID, true, itprId
			, (int)i, 0, vm->m_reg);
		break; // now only one virtual machine sync
	}
	return true;
}


// synchronize symboltable
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncSymbolTable(const int itprId)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;
	sItpr &itpr = m_interpreters[itprId];
	script::cInterpreter *interpreter = itpr.interpreter;

	const uint MaxSyncSymbolCount = 10; // tricky code (todo: streaming)

	if (itpr.symbSyncTime < TIME_SYNC_SYMBOL)
		return true;
	itpr.symbSyncTime = 0.f;

	for (uint i = 0; i < interpreter->m_vms.size(); ++i)
	{
		vector<script::sSyncSymbol> symbols;
		script::cVirtualMachine *vm = interpreter->m_vms[i];

		for (auto &kv1 : vm->m_symbTable.m_vars)
		{
			// tricky code, prevent packet overflow
			if (symbols.size() >= MaxSyncSymbolCount)
				break;

			const string &scope = kv1.first;
			for (auto &kv2 : kv1.second)
			{
				const string &name = kv2.first;
				const string varName = kv1.first + "_" + kv2.first;
				const script::sVariable &var = kv2.second;

				bool isSync = false;
				auto it = m_chSymbols.find(varName);
				if (m_chSymbols.end() == it)
				{
					// add new symbol
					isSync = true;
					sSymbol ssymb;
					ssymb.name = varName;
					ssymb.t = 0;
					ssymb.var = var;
					m_chSymbols[varName] = ssymb;
				}
				else
				{
					// check change? (VT_BYREF (array or map type) crash)
					sSymbol &ssymb = it->second;
					if (!(ssymb.var.var.vt & VT_BYREF) && (ssymb.var.var != var.var))
					{
						isSync = true;
						ssymb.var.var = var.var;
					}
				}

				if (isSync)
				{
					symbols.push_back(script::sSyncSymbol(&scope, &name, &var.var));
					// tricky code, prevent packet overflow
					if (symbols.size() >= MaxSyncSymbolCount)
						break;
				}
			}
		}

		if (!symbols.empty())
		{
			m_protocol.SyncVMSymbolTable(network2::ALL_NETID, true
				, itprId, i, 0, symbols.size(), symbols);

			// continue send symbols
			if (symbols.size() >= MaxSyncSymbolCount)
				itpr.symbSyncTime = TIME_SYNC_SYMBOL;
		}
	}
	
	return true;
}


// clear interpreter array
void cRemoteInterpreter::ClearInterpreters()
{
	for (auto itr : m_interpreters) 
		SAFE_DELETE(itr.interpreter);
	m_interpreters.clear();
}


// request upload intermediate code protocol handler
bool cRemoteInterpreter::UploadIntermediateCode(remotedbg2::UploadIntermediateCode_Packet &packet)
{ 
	// nothing~
	return true; 
}


// request intermeidate code protocol handler
bool cRemoteInterpreter::ReqIntermediateCode(remotedbg2::ReqIntermediateCode_Packet &packet)
{
	if (m_interpreters.size() <= (uint)packet.itprId) {
		// fail, iterpreter id invalid
		m_protocol.AckIntermediateCode(packet.senderId, true, packet.itprId
			, 0, 0, 0, 0, {});
		return true;
	}

	if (m_threads)
	{
		++m_multiThreading;
		m_threads->PushTask(new cSendICodeTask(this, packet.itprId, packet.senderId));
	}
	else
	{
		script::cInterpreter *interpreter = m_interpreters[packet.itprId].interpreter;
		SendIntermediateCode(m_protocol, packet.senderId, packet.itprId, interpreter->m_code);
	}
	return true;
}


// request interpreter run protocol handler
bool cRemoteInterpreter::ReqRun(remotedbg2::ReqRun_Packet &packet)
{
	if (packet.runType == "Run")
		Run(packet.itprId);
	else if (packet.runType == "DebugRun")
		DebugRun(packet.itprId);
	else if (packet.runType == "StepRun")
		StepRun(packet.itprId);

	const int result = IsRun(packet.itprId) ? 1 : 0;
	m_protocol.AckRun(packet.senderId, true, packet.itprId, result);
	SendSyncVMRegister(0);
	return true;
}


// request one stp debugging protocol handler
bool cRemoteInterpreter::ReqOneStep(remotedbg2::ReqOneStep_Packet &packet)
{
	OneStep(packet.itprId);

	// every step debugging, synchronize instruction, register, symboltable information
	if (packet.itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			itpr.regSyncTime = TIME_SYNC_REGISTER + 1.0f;
			itpr.instSyncTime = TIME_SYNC_INSTRUCTION + 1.0f;
			itpr.symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)packet.itprId)
		{
			m_protocol.AckOneStep(packet.senderId, true, packet.itprId, 0);
			return true;
		}

		sItpr &itpr = m_interpreters[packet.itprId];
		itpr.regSyncTime = TIME_SYNC_REGISTER + 1.0f;
		itpr.instSyncTime = TIME_SYNC_INSTRUCTION + 1.0f;
		itpr.symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;
	}

	m_protocol.AckOneStep(packet.senderId, true, packet.itprId, 1);
	return true;
}

// request resume debugging run protocol handler
bool cRemoteInterpreter::ReqResumeRun(remotedbg2::ReqResumeRun_Packet &packet)
{
	Resume(packet.itprId);
	m_protocol.AckResumeRun(packet.senderId, true, packet.itprId, 1);
	return true; 
}

// request interprter break to debugging protocol handler
bool cRemoteInterpreter::ReqBreak(remotedbg2::ReqBreak_Packet &packet)
{
	Break(packet.itprId);
	m_protocol.AckBreak(packet.senderId, true, packet.itprId, 1);
	return true;
}


// request interpreter register breakpoint
bool cRemoteInterpreter::ReqBreakPoint(remotedbg2::ReqBreakPoint_Packet &packet)
{
	BreakPoint(packet.itprId, packet.enable, packet.id);
	m_protocol.AckBreakPoint(packet.senderId, true, packet.itprId, packet.enable, packet.id, 1);
	return true;
}


// request interpreter stop protocol handler
bool cRemoteInterpreter::ReqStop(remotedbg2::ReqStop_Packet &packet)
{
	Stop(packet.itprId);
	m_protocol.AckStop(packet.senderId, true, packet.itprId, 1);
	return true;
}


// request interpreter input event protocol handler
bool cRemoteInterpreter::ReqInput(remotedbg2::ReqInput_Packet &packet)
{
	script::cEvent evt(packet.eventName);
	PushEvent(packet.itprId, evt);
	m_protocol.AckInput(packet.senderId, true, packet.itprId, 1);
	return true;
}


//request interpreter event protocol handler
bool cRemoteInterpreter::ReqEvent(remotedbg2::ReqEvent_Packet &packet) 
{ 
	script::cEvent evt(packet.eventName);
	PushEvent(packet.itprId, evt);
	m_protocol.AckEvent(packet.senderId, true
		, packet.itprId, packet.vmIdx, packet.eventName, 1);
	return true; 
}


// request interpreter step debugging type
bool cRemoteInterpreter::ReqStepDebugType(remotedbg2::ReqStepDebugType_Packet &packet)
{
	for (auto &itpr : m_interpreters)
		itpr.interpreter->SetICodeStepDebug(packet.stepDbgType == 1);

	m_protocol.AckStepDebugType(packet.senderId, true, packet.stepDbgType, 1);
	return true;
}


// is webserver connected?
bool cRemoteInterpreter::IsConnect()
{
	return m_server.IsConnect();
}


// is webserver fail connection?
bool cRemoteInterpreter::IsFailConnect()
{
	return m_server.IsFailConnection();
}


// clear all data
void cRemoteInterpreter::Clear()
{
	//m_state = eState::Stop;
	m_chSymbols.clear();
	m_server.Close();
	ClearInterpreters();
	m_threads = nullptr;
}


namespace {
	common::cMemoryPool2<1024 * 100> g_memPool;
}

// send intermediate code, using remotedbg2 protocol
bool SendIntermediateCode(remotedbg2::h2r_Protocol &protocol
	, const netid recvId, const int itprId, const script::cIntermediateCode &icode)
{
	// marshalling intermedatecode to byte stream
	const uint BUFFER_SIZE = 1024 * 100;
	BYTE *buff = (BYTE*)g_memPool.Alloc();

	cPacket marsh(network2::GetPacketHeader(ePacketFormat::JSON));
	marsh.m_data = buff;
	marsh.m_bufferSize = BUFFER_SIZE;
	marsh.m_writeIdx = 0;
	network2::marshalling::operator<<(marsh, icode);

	// send split
	const uint bufferSize = (uint)marsh.m_writeIdx;
	if (bufferSize == 0)
	{
		// no intermediate code, fail!
		protocol.AckIntermediateCode(recvId, true, itprId, 0, 0, 0, 0, {});
	}
	else
	{
		// 4 bytes = itpr, result, count, index
		// 2 * 4byte = totalBufferSize, buffsize
		// 12 bytes = 4 + 8
		const uint CHUNK_SIZE = network2::DEFAULT_PACKETSIZE -
			(marsh.GetHeaderSize() + 12);
		const uint totalCount = ((bufferSize % CHUNK_SIZE) == 0) ?
			bufferSize / CHUNK_SIZE : bufferSize / CHUNK_SIZE + 1;

		uint index = 0;
		uint cursor = 0;
		while (cursor < bufferSize)
		{
			const uint size = std::min(bufferSize - cursor, CHUNK_SIZE);
			vector<BYTE> data(size);
			memcpy_s(&data[0], size, &marsh.m_data[cursor], size);
			protocol.AckIntermediateCode(recvId, true
				, (BYTE)itprId, 1, (BYTE)totalCount, (BYTE)index, bufferSize, data);

			cursor += size;
			++index;
		}
	}

	g_memPool.Free(buff);
	return true;
}