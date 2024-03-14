
#include "stdafx.h"
#include "remoteinterpreter.h"

using namespace network2;


//----------------------------------------------------------------------------------
// Send Intermediate Code Task
bool SendIntermediateCode(remotedbg2::h2r_Protocol &protocol
	, const netid recvId, const int itprId, const int vmId
	, const script::cIntermediateCode &icode);

class cSendICodeTask : public common::cTask
{
public:
	cSendICodeTask(cRemoteInterpreter *rmtItpr, const int itprId, const int vmId
		, const netid rcvId)
		: cTask(1, "cSendICodeTask")
		, m_rmtItpr(rmtItpr), m_itprId(itprId), m_vmId(vmId), m_rcvId(rcvId) {
	}
	virtual eRunResult Run(const double deltaSeconds)
	{
		script::cInterpreter* interpreter = nullptr;
		script::cVirtualMachine* vm = nullptr;

		if (m_rmtItpr->m_interpreters.size() <= (uint)m_itprId)
			goto $error1;
		interpreter = m_rmtItpr->m_interpreters[m_itprId].interpreter;
		vm = interpreter->GetVM(m_vmId);
		if (!vm)
			goto $error2;

		SendIntermediateCode(m_rmtItpr->m_protocol, m_rcvId, m_itprId, m_vmId
			, vm->m_code);
		--m_rmtItpr->m_multiThreading;
		return eRunResult::End;


	$error1:
		// fail, iterpreter id invalid
		dbg::Logc(1, "error send icode, not found interpreter, itprId: %d, vmId: %d\n"
			, m_itprId, m_vmId);
		m_rmtItpr->m_protocol.AckIntermediateCode(m_rcvId, true, m_itprId, m_vmId
			, 0, 0, 0, 0, {});
		--m_rmtItpr->m_multiThreading;
		return eRunResult::End;

	$error2:
		// fail, vm id invalid
		//dbg::Logc(1, "error send icode, not found vm, itprId: %d, vmId: %d\n"
		//	, m_itprId, m_vmId);
		m_rmtItpr->m_protocol.AckIntermediateCode(m_rcvId, true, m_itprId, m_vmId
			, 0, 0, 0, 0, {});
		--m_rmtItpr->m_multiThreading;
		return eRunResult::End;
	}

public:
	cRemoteInterpreter *m_rmtItpr; // reference
	int m_itprId; // interpreter id
	int m_vmId; // virtual machine id
	netid m_rcvId;
};


//----------------------------------------------------------------------------------
// cRemoteInterpreter
cRemoteInterpreter::cRemoteInterpreter(
	const StrId &name
	, const int logId //= -1
)
	: m_server(new network2::cWebSessionFactory(), name, logId)
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
	, const bool isSpawnHttpSvr //= true
)
{
	Clear();

	m_bindPort = bindPort;
	m_isThreadMode = isThreadMode;

	m_server.AddProtocolHandler(this);
	m_server.RegisterProtocol(&m_protocol);

	if (!netController.StartWebServer(&m_server, bindPort, packetSize
		, maxPacketCount, sleepMillis, isThreadMode, isSpawnHttpSvr))
	{
		dbg::Logc(2, "Error Start WebServer port:%d \n", bindPort);
		return false;
	}

	return true;
}


// load intermediate code
// fileName: intermediate code filename
// return virtual machine id, -1:error
int cRemoteInterpreter::LoadIntermediateCode(const StrPath &fileName
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init();
	interpreter->SetListener(this);

	const int vmId = interpreter->LoadIntermediateCode(fileName, parentVmId, scopeName);
	if (vmId < 0)
	{
		delete interpreter;
		return -1;
	}

	sItpr itpr;
	itpr.name = common::format("itpr%d", m_interpreters.size());// interpreter->m_fileName.c_str();
	itpr.state = sItpr::eState::Stop;
	itpr.interpreter = interpreter;

	m_interpreters.push_back(itpr);
	return vmId;
}


// load intermediate code 
// fileNames: intermediate code filename array
// parentVmId: parent virtual machine id, -1:root
bool cRemoteInterpreter::LoadIntermediateCode(const vector<StrPath> &fileNames
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	ClearInterpreters();
	m_interpreters.reserve(fileNames.size());

	bool isSuccess = true;
	for (auto &fileName : fileNames)
	{
		if (LoadIntermediateCode(fileName, parentVmId, scopeName) < 0)
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
// vmName: virtual machine name
// parentVmId: parent virtual machine id, -1:root
// return: virutal machine id, -1:error
int cRemoteInterpreter::LoadIntermediateCode(
	const common::script::cIntermediateCode &icode
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init();
	interpreter->SetListener(this);

	const int vmId = interpreter->LoadIntermediateCode(icode, parentVmId, scopeName);
	if (vmId < 0)
	{
		delete interpreter;
		return false;
	}

	sItpr itpr;
	itpr.name = common::format("itpr%d", m_interpreters.size()); //interpreter->m_fileName.c_str();
	itpr.state = sItpr::eState::Stop;
	itpr.interpreter = interpreter;

	m_interpreters.push_back(itpr);
	return vmId;
}


// add virtual machine with intermediate code
// parentVmId: parent virtual machine id, -1:root
// return: vm id, -1:error
int cRemoteInterpreter::AddVM(const int itprId
	, const common::script::cIntermediateCode& icode
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	if (m_interpreters.size() <= (uint)itprId)
		return -1;
	sItpr &itpr = m_interpreters[itprId];
	return itpr.interpreter->LoadIntermediateCode(icode, parentVmId, scopeName);
}


// add function execute module
bool cRemoteInterpreter::AddModule(common::script::iModule *mod)
{
	auto it = std::find(m_modules.begin(), m_modules.end(), mod);
	if (m_modules.end() != it)
		return false; // already exist
	m_modules.push_back(mod);
	return true;
}


// remove function execute module
bool cRemoteInterpreter::RemoveModule(common::script::iModule *mod)
{
	common::removevector(m_modules, mod);
	return true;
}


// send sync vm register, instruction, symbol
// update latest information with interpreter client
bool cRemoteInterpreter::SendSyncAll()
{
	for (uint k = 0; k < m_interpreters.size(); ++k)
	{
		const int itprId = (int)k;
		sItpr &itpr = m_interpreters[itprId];
		SendSyncInstruction(itprId);
		SendSyncVMRegister(itprId);
		SendSyncSymbolTable(itprId, -1);
	}
	return true;
}


// process with webclient, interpreter
// procCnt: execute instruction count
bool cRemoteInterpreter::Process(const float deltaSeconds
	, const uint procCnt //= 1
)
{
	float dt = deltaSeconds;
	uint cnt = 0;
	while (procCnt > cnt++)
	{
		// execute interpreter
		for (uint i = 0; i < m_interpreters.size(); ++i)
		{
			const int itprId = (int)i;
			sItpr& itpr = m_interpreters[itprId];
			itpr.interpreter->Process(dt);

			if (itpr.interpreter->IsBreak())
				m_protocol.AckOneStep(network2::ALL_NETID, true, i, 1);
		}
		dt = 0.f; // clear
	}

	// synchronize vm info
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		sItpr& itpr = m_interpreters[i];
		if (sItpr::eState::Run == itpr.state)
		{
			SendSyncVMRegister(i);
			SendSyncInstruction(i);
			SendSyncSymbolTable(i, -1);
		}
	}

	return m_server.IsFailConnection() ? false : true;
}


// push interpreter event, wrapping function
// itprId: interpreter id, -1: all interpreter, all vm
// vmId: virtual machine id, -1: all vm
// return: success push
bool cRemoteInterpreter::PushEvent(const int itprId
	, const int vmId, shared_ptr<script::cEvent> evt
)
{
	bool res = false;
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (sItpr::eState::Run == itpr.state)
				if (interpreter->PushEvent(-1, evt))
					res = true;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;

		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		if (interpreter->PushEvent(vmId, evt))
			res = true;
	}
	return res;
}


// start interpreter
// itprId: interpreter index, -1: all interpreter
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
// nodeName: RunNodeFile node name (scopeName)
bool cRemoteInterpreter::Run(const int itprId
	, const int parentVmId //= -1
	, const int vmId //= -1
	, const script::cSymbolTable& symbTable //= {}
	, const string& nodeName //= ""
	, const string& startEvent //= "Start Event"
)
{
	return RunInterpreter(itprId, parentVmId, vmId, symbTable, nodeName, startEvent, 0);
}


// start interpreter with debug run
// itprId: interpreter index, -1: all interpreter
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
// nodeName: RunNodeFile node name (scopeName)
bool cRemoteInterpreter::DebugRun(const int itprId
	, const int parentVmId //= -1
	, const int vmId //= -1
	, const script::cSymbolTable& symbTable //= {}
	, const string& nodeName //= ""
	, const string& startEvent //= "Start Event"
)
{
	return RunInterpreter(itprId, parentVmId, vmId, symbTable, nodeName, startEvent, 1);
}


// start interpreter with one step debugging
// itprId: interpreter index, -1: all interpreter
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
// nodeName: RunNodeFile node name (scopeName)
bool cRemoteInterpreter::StepRun(const int itprId
	, const int parentVmId //= -1
	, const int vmId //= -1
	, const script::cSymbolTable& symbTable //= {}
	, const string& nodeName //= ""
	, const string& startEvent //= "Start Event"
)
{
	return RunInterpreter(itprId, parentVmId, vmId, symbTable, nodeName, startEvent, 2);
}


// terminate virtual machine
// vmId: terminate virtual machine id
// return: success?
bool cRemoteInterpreter::TerminateVM(const int vmId)
{
	sItpr *itpr = GetInterpreterByVMId(vmId);
	if (!itpr)
		return false;	
	const bool result = itpr->interpreter->Terminate(vmId);
	return result;
}


// stop interpreter
// itprId: interpreter index, -1: all interpreter
// vmId: virtual machine id, -1:all vm
bool cRemoteInterpreter::Stop(const int itprId
	, const int vmId //= -1
)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			interpreter->Stop(-1);
			itpr.state = sItpr::eState::Stop;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;

		sItpr &itpr = m_interpreters[itprId];
		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Stop(vmId);
		if (vmId < 0) // all vm?
			itpr.state = sItpr::eState::Stop;
	}
	return true;
}


// resume run interpreter
// itprId: interpreter index
// vmId: virtual machine id, -1:all vm
bool cRemoteInterpreter::Resume(const int itprId
	, const int vmId //= -1
)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (sItpr::eState::Run == itpr.state)
				interpreter->Resume(-1);
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (sItpr::eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		return interpreter->Resume(vmId);
	}
	return true;
}


// onestep interpreter if debug mode
// itprId: interpreter index
// vmId: virtual machine id, -1:all vm
bool cRemoteInterpreter::OneStep(const int itprId
	, const int vmId //= -1
)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (sItpr::eState::Run == itpr.state)
				interpreter->OneStep(-1);
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (sItpr::eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		return interpreter->OneStep(vmId);
	}
	return true;
}


// break(pause) interpreter
// itprId: interpreter index
// vmId: virtual machine id, -1:all vm
bool cRemoteInterpreter::Break(const int itprId
	, const int vmId //= -1
)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (sItpr::eState::Run == itpr.state)
				interpreter->Break(-1);
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr &itpr = m_interpreters[itprId];
		if (sItpr::eState::Run != itpr.state)
			return false;

		script::cInterpreter *interpreter = itpr.interpreter;
		interpreter->Break(vmId);
	}
	return true;
}


// set break point
// itprId: interpreter index
bool cRemoteInterpreter::BreakPoint(const int itprId, const int vmId
	, const bool enable, const uint id)
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
		interpreter->BreakPoint(enable, vmId, id);
	}
	return true;
}


// synchronize interpreter information (only symboltable)
// itprId: interpreter index
// vmId: virtual machine id, -1:all vm
bool cRemoteInterpreter::SyncInformation(const int itprId, const int vmId)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;
	sItpr& itpr = m_interpreters[itprId];

	if (vmId < 0)
	{
		for (auto& vm : itpr.interpreter->m_vms)
		{
			vm->m_nsync.sync = 0x4; // instant symbol synchronize
			vm->m_nsync.symbStreaming = true;
		}
	}
	else
	{
		if (script::cVirtualMachine* vm = GetVM(vmId))
		{
			vm->m_nsync.sync = 0x4; // instant symbol synchronize
			vm->m_nsync.symbStreaming = true;
		}
	}

	SendSyncSymbolTable(itprId, vmId);
	return true;
}


// synchronize virtual machine information (only symboltable)
// vmId: virtual machine id
// varNames: synchronize variable name array 'scopeName;varName'
bool cRemoteInterpreter::SyncVMInformation(const int vmId, const vector<string>& varNames)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	if (itprId < 0)
		return false; // error return

	sItpr& itpr = m_interpreters[itprId];

	if (vmId < 0)
	{
		for (auto& vm : itpr.interpreter->m_vms)
		{
			vm->m_nsync.sync = 0x4; // instant symbol synchronize
			vm->m_nsync.symbStreaming = true;
		}
	}
	else
	{
		if (script::cVirtualMachine* vm = GetVM(vmId))
		{
			vm->m_nsync.sync = 0x04; // instant symbol synchronize
			vm->m_nsync.symbStreaming = true;
		}
	}

	if (IsChangeSymbolTable(itprId))
	{
		SendSyncSymbolTable(itprId, vmId);
	}

	// sync specific variable
	for (uint i=0; i < itpr.interpreter->m_vms.size(); ++i)
	{
		auto& vm = itpr.interpreter->m_vms[i];
		if (vm->m_id != vmId) continue;

		for (auto& varName : varNames)
		{
			vector<string> toks;
			common::tokenizer(varName, ";", "", toks);
			if (toks.size() != 2)
				continue; // error
			script::sVariable *var = vm->m_symbTable.FindVarInfo(toks[0], toks[1]);
			if (!var)
				continue; // not found, error

			if (var->IsReference())
			{
				SendSyncVariable(itprId, vm->m_id, vm->m_symbTable
					, script::cSymbolTable::MakeScopeName3(toks[0], toks[1]), *var);

				auto it = vm->m_symbTable.m_varMap.find(var->var.intVal);
				if (vm->m_symbTable.m_varMap.end() == it)
					continue; // error
				var = vm->m_symbTable.FindVarInfo(it->second.first, it->second.second);
				if (!var)
					continue; // not found, error
				SendSyncVariable(itprId, vm->m_id, vm->m_symbTable
					, script::cSymbolTable::MakeScopeName3(it->second.first, it->second.second)
					, *var);
			}
			else if (var->IsArray())
			{
				SendSyncVariable(itprId, vm->m_id, vm->m_symbTable
					, script::cSymbolTable::MakeScopeName3(toks[0], toks[1]), *var);
			}
			else if (var->IsMap())
			{
				SendSyncVariable(itprId, vm->m_id, vm->m_symbTable
					, script::cSymbolTable::MakeScopeName3(toks[0], toks[1]), *var);
			}
		}
	}
	return true;
}


// is run or debug run interpreter?
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::IsRunning(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			const bool res = (sItpr::eState::Run == itpr.state) &&
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
		return (sItpr::eState::Run == itpr.state) 
			&& (interpreter->IsRun() || interpreter->IsDebugRun());
	}
	return false;
}


// is run interpreter?
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::IsRun(const int itprId)
{
	if (itprId < 0)
	{
		for (auto& itpr : m_interpreters)
		{
			script::cInterpreter* interpreter = itpr.interpreter;
			const bool res = (sItpr::eState::Run == itpr.state) &&
				interpreter->IsRun();
			if (res)
				return true;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr& itpr = m_interpreters[itprId];
		script::cInterpreter* interpreter = itpr.interpreter;
		return (sItpr::eState::Run == itpr.state)
			&& interpreter->IsRun();
	}
	return false;
}


// debug run interpreter?
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::IsDebugRun(const int itprId)
{
	if (itprId < 0)
	{
		for (auto& itpr : m_interpreters)
		{
			script::cInterpreter* interpreter = itpr.interpreter;
			const bool res = (sItpr::eState::Run == itpr.state) &&
				interpreter->IsDebugRun();
			if (res)
				return true;
		}
	}
	else
	{
		if (m_interpreters.size() <= (uint)itprId)
			return false;
		sItpr& itpr = m_interpreters[itprId];
		script::cInterpreter* interpreter = itpr.interpreter;
		return (sItpr::eState::Run == itpr.state) && interpreter->IsDebugRun();
	}
	return false;
}


// is debugging interpreter?
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::IsDebugging(const int itprId)
{
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			const bool res = (sItpr::eState::Run == itpr.state) && interpreter->IsDebug();
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
		return (sItpr::eState::Run == itpr.state) && interpreter->IsDebug();
	}
	return false;
}


// find interpreter id (index) by virtual machine id
// vmId: virtual machine id
// return: interpreter id (index), -1: not found
int cRemoteInterpreter::GetInterpreterIdByVMId(const int vmId)
{
	int itprId = -1;
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		network2::cRemoteInterpreter::sItpr& info = m_interpreters[i];
		script::cInterpreter* itpr = info.interpreter;
		if (!itpr)
			continue;
		auto it = std::find_if(itpr->m_vms.begin(), itpr->m_vms.end()
			, [&](const auto& a) { return a->m_id == vmId; });
		if (itpr->m_vms.end() != it)
		{
			itprId = (int)i;
			break;
		}
	}
	return itprId;
}


// find interpreter id (index) by remove virtual machine id
// vmId: virtual machine id
// return: interpreter id (index), -1: not found
int cRemoteInterpreter::GetInterpreterIdByRemoveVMId(const int vmId)
{
	int itprId = -1;
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		network2::cRemoteInterpreter::sItpr& info = m_interpreters[i];
		script::cInterpreter* itpr = info.interpreter;
		if (!itpr)
			continue;
		auto it = std::find_if(itpr->m_rmVms.begin(), itpr->m_rmVms.end()
			, [&](const auto& a) { return a->m_id == vmId; });
		if (itpr->m_rmVms.end() != it)
		{
			itprId = (int)i;
			break;
		}
	}
	return itprId;
}


// return virtual machine by vmid
script::cVirtualMachine* cRemoteInterpreter::GetVM(const int vmId)
{
	script::cVirtualMachine* vm = nullptr;
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		network2::cRemoteInterpreter::sItpr& info = m_interpreters[i];
		if (vm = info.interpreter->GetVM(vmId))
			break;
	}
	return vm;
}


// return virtual machine by vmid
script::cVirtualMachine* cRemoteInterpreter::GetRemoveVM(const int vmId)
{
	script::cVirtualMachine* vm = nullptr;
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		network2::cRemoteInterpreter::sItpr& info = m_interpreters[i];
		if (vm = info.interpreter->GetRemoveVM(vmId))
			break;
	}
	return vm;
}


// terminate virtual machine callback function
void cRemoteInterpreter::TerminateResponse(const int vmId)
{
	// send synchronize info with remove virtual machine last instruction
	if (script::cVirtualMachine* vm = GetRemoveVM(vmId))
	{
		const int itprId = GetInterpreterIdByRemoveVMId(vmId);
		if (itprId >= 0)
			SendSyncVMInstruction(itprId, vm);
	}
	//~

	m_protocol.RemoveInterpreter(network2::ALL_NETID, true, vmId);
}


// start timeout response
void cRemoteInterpreter::SetTimeOutResponse(const int vmId, const string& scopeName, const int timerId, const int time)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, scopeName, timerId, time, 1);
}


// clear timeout response
void cRemoteInterpreter::ClearTimeOutResponse(const int vmId, const int timerId, const int id)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, "", timerId, 0, 0);
}


// start interval response
void cRemoteInterpreter::SetIntervalResponse(const int vmId, const string& scopeName, const int timerId, const int time)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, scopeName, timerId, time, 2);
}


// clear interval  response
void cRemoteInterpreter::ClearIntervalResponse(const int vmId, const int timerId, const int id)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, "", timerId, 0, 0);
}


// start sync timeout response
void cRemoteInterpreter::SyncTimeOutResponse(const int vmId, const string& scopeName, const int timerId, const int time)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, scopeName, timerId, time, 3);
}


// clear sync timeout response
void cRemoteInterpreter::ClearSyncTimeOutResponse(const int vmId, const int timerId, const int id)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMTimer(network2::ALL_NETID, true, itprId, vmId, "", timerId, 0, 0);
}


// error message from virtual machine
void cRemoteInterpreter::ErrorVM(const int vmId, const string& msg)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	m_protocol.SyncVMOutput(network2::ALL_NETID, true, itprId, vmId, msg);
}


// find interpreter by virtual machine id
// vmId: virtual machine id
cRemoteInterpreter::sItpr* cRemoteInterpreter::GetInterpreterByVMId(const int vmId)
{
	for (uint i = 0; i < m_interpreters.size(); ++i)
	{
		network2::cRemoteInterpreter::sItpr& info = m_interpreters[i];
		script::cInterpreter* itpr = info.interpreter;
		if (!itpr)
			continue;
		auto it = std::find_if(itpr->m_vms.begin(), itpr->m_vms.end()
			, [&](const auto& a) { return a->m_id == vmId; });
		if (itpr->m_vms.end() != it)
			return &info;
	}
	return nullptr;
}


// run interpreter
// itprId: interpreter id, -1:all interpreter
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
// nodeName: RunNodeFile node name (scopeName)
// type: 0:run, 1:debug run, 2:step run
bool cRemoteInterpreter::RunInterpreter(const int itprId, const int parentVmId, 
	const int vmId, const script::cSymbolTable& symbTable
	, const string& nodeName, const string& startEvent, const int type)
{
	if (itprId < 0)
	{
		for (uint i = 0; i < m_interpreters.size(); ++i)
			RunInterpreter_Sub(i, parentVmId, vmId, symbTable, nodeName, startEvent, type);
	}
	else
	{
		return RunInterpreter_Sub(itprId, parentVmId, vmId, symbTable, nodeName, startEvent, type);
	}
	return true;
}


// run interpreter
// itprId: interpreter id
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
// nodeName: RunNodeFile node name (scopeName)
// type: 0:run, 1:debug run, 2:step run
bool cRemoteInterpreter::RunInterpreter_Sub(const int itprId, const int parentVmId
	, const int vmId, const script::cSymbolTable& symbTable
	, const string& nodeName, const string& startEvent, const int type)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;

	sItpr& itpr = m_interpreters[itprId];
	if ((vmId < 0) && (sItpr::eState::Stop != itpr.state))
		return false; // error, already run

	bool result = false;
	script::cInterpreter* interpreter = itpr.interpreter;
	interpreter->SetCodeTrace(true);
	for (auto& mod : m_modules)
		interpreter->AddModule(mod);

	switch (type)
	{
	case 0: // Run
		if (result = interpreter->Run(vmId, symbTable, startEvent))
		{
			itpr.state = sItpr::eState::Run;
			SendSpawnInterpreterInfo(itprId, parentVmId, vmId, nodeName);
			SyncInformation(itprId, vmId);
			return true;
		}
		break;
	case 1: // DebugRun
		if (result = interpreter->DebugRun(vmId, symbTable, startEvent))
		{
			itpr.state = sItpr::eState::Run;
			SendSpawnInterpreterInfo(itprId, parentVmId, vmId, nodeName);
			SyncInformation(itprId, vmId);
		}
		break;
	case 2: // StepRun
		if (result = interpreter->StepRun(vmId, symbTable, startEvent))
		{
			itpr.state = sItpr::eState::Run;
			SendSpawnInterpreterInfo(itprId, parentVmId, vmId, nodeName);
		}
		break;
	default:
		return false;
	}
	return result;
}


// check symboltable change
// itprId: interpreter id
bool cRemoteInterpreter::IsChangeSymbolTable(const int itprId)
{
	return IsChangeSymbolTable(m_interpreters[itprId]);
}
bool cRemoteInterpreter::IsChangeSymbolTable(const sItpr& itpr)
{
	script::cInterpreter* interpreter = itpr.interpreter;
	for (uint i = 0; i < interpreter->m_vms.size(); ++i)
	{
		script::cVirtualMachine* vm = interpreter->m_vms[i];
		if (vm->m_symbTable.m_isChange)
			return true;
	}
	return false;
}


// send spawn interpreter info
// itprId: interpreter id
// parentVmId: execute parent virtual machine id
// vmId: spawn virtual machine id
// nodeName: RunNodeFile node name
bool cRemoteInterpreter::SendSpawnInterpreterInfo(const int itprId, const int parentVmId
	, const int vmId, const string &nodeName)
{
	sItpr& itpr = m_interpreters[itprId];
	if (vmId < 0)
	{
		for (auto& vm : itpr.interpreter->m_vms)
		{
			// remove *.icode postfix string
			string fileName = vm->m_code.m_fileName.c_str();
			const int pos = fileName.rfind(".icode");
			if (pos != string::npos)
				fileName = fileName.substr(0, pos);
			//

			m_protocol.SpawnInterpreterInfo(network2::ALL_NETID, true, itprId, parentVmId
				, vm->m_id, fileName, nodeName);
		}
	}
	else
	{
		script::cVirtualMachine *vm = itpr.interpreter->GetVM(vmId);
		if (!vm)
			return false; // error return

		// remove *.icode postfix string
		string fileName = vm->m_code.m_fileName.c_str();
		const int pos = fileName.rfind(".icode");
		if (pos != string::npos)
			fileName = fileName.substr(0, pos);
		//

		m_protocol.SpawnInterpreterInfo(network2::ALL_NETID, true, itprId, parentVmId
			, vmId, fileName, nodeName);
	}

	return true;
}


// sync instruction set
// itprId: interpreter id
bool cRemoteInterpreter::SendSyncInstruction(const int itprId)
{
	sItpr &itpr = m_interpreters[itprId];
	script::cInterpreter *interpreter = itpr.interpreter;
	for (uint i = 0; i < interpreter->m_vms.size(); ++i)
	{
		script::cVirtualMachine *vm = interpreter->m_vms[i];
		if (!(vm->m_nsync.enable || (vm->m_nsync.sync & 0x02)) 
			|| !vm->m_nsync.instStreaming || vm->m_trace.empty())
			continue; // no process
		SendSyncVMInstruction(itprId, vm);
		vm->m_nsync.sync = (vm->m_nsync.sync & ~0x02); // clear flag
		vm->m_nsync.instStreaming = false; // clear flag
	}
	return true;
}


// sync vm register info
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncVMRegister(const int itprId)
{
	script::cInterpreter *interpreter = m_interpreters[itprId].interpreter;
	for (uint i=0; i < interpreter->m_vms.size(); ++i)
	{
		script::cVirtualMachine *vm = interpreter->m_vms[i];
		if ((vm->m_nsync.enable || (vm->m_nsync.sync & 0x01)) && vm->m_nsync.regStreaming)
		{
			m_protocol.SyncVMRegister(network2::ALL_NETID, true, itprId
				, vm->m_id, 0, vm->m_reg);
			vm->m_nsync.sync = vm->m_nsync.sync & ~0x01; // clear flag
			vm->m_nsync.regStreaming = false; // clear flag
		}
		//break; // now only one virtual machine sync
	}
	return true;
}


// synchronize symboltable
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncSymbolTable(const int itprId, const int vmId)
{
	sItpr &itpr = m_interpreters[itprId];
	script::cInterpreter *interpreter = itpr.interpreter;

	const uint MaxSyncSymbolCount = 10; // tricky code (todo: streaming)

	for (uint i = 0; i < interpreter->m_vms.size(); ++i)
	{
		vector<script::sSyncSymbol> symbols;
		script::cVirtualMachine *vm = interpreter->m_vms[i];
		if ((vmId >= 0) && (vm->m_id != vmId))
			continue; // ignore this vm
		if (!(vm->m_nsync.enable || (vm->m_nsync.sync & 0x04)) || !vm->m_nsync.symbStreaming)
			continue;

		for (auto &kv1 : vm->m_symbTable.m_vars)
		{
			const string &scopeName = kv1.first;
			for (auto &kv2 : kv1.second)
			{
				const string &name = kv2.first;
				const string varName = scopeName + "_" + kv2.first;
				const script::sVariable &var = kv2.second;

				bool isSync = false;
				auto it = vm->m_nsync.chSymbols.find(varName);
				if (vm->m_nsync.chSymbols.end() == it)
				{
					// add new symbol
					// tricky codee: prevent sVariable deepcopy (must shallow copy)
					isSync = true;
					script::cVirtualMachine::sSymbol &ssymb = vm->m_nsync.chSymbols[varName];
					ssymb.name = varName;
					ssymb.t = 0.f;
					ssymb.var.ShallowCopy(var);
				}
				else
				{
					// check change?
					script::cVirtualMachine::sSymbol& ssymb = it->second;
					if (((ssymb.var.var.vt & VT_BYREF) && (ssymb.var.var.intVal != var.var.intVal))
						|| (!(ssymb.var.var.vt & VT_BYREF) && (ssymb.var.var != var.var)))
					{
						isSync = true;
						ssymb.var.var = var.var;
					}
				}

				if (isSync)
				{
					symbols.push_back(script::sSyncSymbol(&scopeName, &name, &var.var));
				}
			}
		}

		if (!symbols.empty())
		{
			if (symbols.size() > MaxSyncSymbolCount)
			{
				uint offset = 0;
				while (symbols.size() > offset)
				{
					const uint start = offset;
					vector<script::sSyncSymbol> s;
					for (uint k = 0; 
						(k < MaxSyncSymbolCount) && (offset < symbols.size());
						++k, ++offset)
						s.push_back(symbols[offset]);

					m_protocol.SyncVMSymbolTable(network2::ALL_NETID, true
						, itprId, vm->m_id, start, s.size(), s);
				}
			}
			else
			{
				m_protocol.SyncVMSymbolTable(network2::ALL_NETID, true
					, itprId, vm->m_id, 0, symbols.size(), symbols);
			}
		}
		vm->m_symbTable.SyncChange(); // clear change flag
		vm->m_nsync.sync = vm->m_nsync.sync & ~0x04; // clear flag
		vm->m_nsync.symbStreaming = false; // clear flag
	}//~for
	
	return true;
}


// send specific variable sync
// only sync array, map type 
bool cRemoteInterpreter::SendSyncVariable(const int itprId, const int vmId
	, const script::cSymbolTable& symbolTable
	, const string &varName, const script::sVariable& var)
{
	const uint headerSize = 20; // bin/json header size, hard coding
	const uint maxPacketSize = network2::DEFAULT_PACKETSIZE;

	if (var.IsArray())
	{
		if (var.GetArraySize() == 0)
			return true; 

		switch (var.ar[0].vt)
		{
		case VT_BOOL:
		{
			uint i = 0;
			while (i < var.arSize)
			{
				uint offset = headerSize + 12 + varName.size() + 1 + 4 + 4; //+4:dummy
				const uint startIdx = i;
				vector<bool> values;
				values.reserve(var.arSize);
				for (; i < var.arSize; ++i)
				{
					values.push_back((bool)var.ar[i]);
					offset += sizeof(bool);
					if (offset + sizeof(bool) > maxPacketSize)
						break;
				}
				m_protocol.SyncVMArrayBool(network2::ALL_NETID, true, itprId, vmId
					, varName, startIdx, values);
			}
		}
		break;

		case VT_INT:
		case VT_R4:
		{
			uint i = 0;
			while (i < var.arSize)
			{
				uint offset = headerSize + 12 + varName.size() + 1 + 4 + 4; //+4:dummy
				const uint startIdx = i;
				vector<float> values;
				values.reserve(var.arSize);
				for (; i < var.arSize; ++i)
				{
					values.push_back((float)var.ar[i]);
					offset += sizeof(float);
					if (offset + sizeof(float) > maxPacketSize)
						break;
				}
				m_protocol.SyncVMArrayNumber(network2::ALL_NETID, true, itprId, vmId
					, varName, startIdx, values);
			}
		}
		break;

		case VT_BSTR:
		{
			uint i = 0;
			while (i < var.arSize)
			{
				uint offset = headerSize + 12 + varName.size() + 1 + 4 + 4; //+4:dummy
				const uint startIdx = i;
				vector<string> values;
				values.reserve(var.arSize);
				for (; i < var.arSize; ++i)
				{
					const string str = (string)(bstr_t)var.ar[i];
					values.push_back(str);
					offset += str.size() + 1;
					if (offset + sizeof(float) > maxPacketSize)
						break;
				}
				m_protocol.SyncVMArrayString(network2::ALL_NETID, true, itprId, vmId
					, varName, startIdx, values);
			}
		}
		break;

		default:
			break;
		}
	}
	else if (var.IsMap())
	{
		// todo: another type synchronize
	}
	else
	{
		// todo: another type synchronize
	}

	return true;
}


// send synchronize virtual machine instruction info
bool cRemoteInterpreter::SendSyncVMInstruction(const int itprId, script::cVirtualMachine* vm)
{
	const uint size = vm->m_trace.size();
	if ((size == 2) && (vm->m_trace[0] == vm->m_trace[1]))
		return false; // no process, no changed

	// exception: check buffer overflow
	// too many execute instruction, limit 200 size, 300x2byte = 600byte
	const uint MAX_TRACE_SIZE = 300;
	if (vm->m_trace.size() < MAX_TRACE_SIZE)
	{
		m_protocol.SyncVMInstruction(network2::ALL_NETID
			, true, itprId, vm->m_id, vm->m_trace);
		vm->ClearCodeTrace(true);
	}
	else
	{
		// too many code execute, split trace data
		vector<ushort> trace;
		trace.reserve(MAX_TRACE_SIZE);
		for (uint i = 0; i < MAX_TRACE_SIZE; ++i)
			trace.push_back(vm->m_trace[i]);

		m_protocol.SyncVMInstruction(network2::ALL_NETID
			, true, itprId, vm->m_id, trace);
		vm->ClearCodeTrace(true, MAX_TRACE_SIZE);
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
	if (m_interpreters.size() <= (uint)packet.itprId)
		goto $error;

	if (m_threads)
	{
		++m_multiThreading;
		m_threads->PushTask(new cSendICodeTask(this, packet.itprId, packet.vmId, packet.senderId));
	}
	else
	{
		script::cInterpreter *interpreter = m_interpreters[packet.itprId].interpreter;
		script::cVirtualMachine *vm = interpreter->GetVM(packet.vmId);
		if (!vm)
			goto $error;

		SendIntermediateCode(m_protocol, packet.senderId, packet.itprId, packet.vmId, vm->m_code);
	}
	return true;


$error:
	// fail, iterpreter id invalid
	m_protocol.AckIntermediateCode(packet.senderId, true, packet.itprId, packet.vmId
		, 0, 0, 0, 0, {});
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

	const int result = IsRunning(packet.itprId) ? 1 : 0;
	m_protocol.AckRun(packet.senderId, true, packet.itprId, result);
	for (uint i=0; i < m_interpreters.size(); ++i)
		SendSyncVMRegister((int)i);
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
			for (auto& vm : itpr.interpreter->m_vms)
			{
				vm->m_nsync.sync = 0x0F; // instant sync
				vm->m_nsync.regStreaming = true;
				vm->m_nsync.instStreaming = true;
				vm->m_nsync.symbStreaming = true;
			}
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
		for (auto& vm : itpr.interpreter->m_vms)
		{
			vm->m_nsync.sync = 0x0F; // instant sync
			vm->m_nsync.regStreaming = true;
			vm->m_nsync.instStreaming = true;
			vm->m_nsync.symbStreaming = true;
		}
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
	m_protocol.AckBreak(packet.senderId, true, packet.itprId, packet.vmId, 1);
	return true;
}


// request interpreter register breakpoint
bool cRemoteInterpreter::ReqBreakPoint(remotedbg2::ReqBreakPoint_Packet &packet)
{
	BreakPoint(packet.itprId, packet.vmId, packet.enable, packet.id);
	m_protocol.AckBreakPoint(packet.senderId, true, packet.itprId, packet.vmId
		, packet.enable, packet.id, 1);
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
	PushEvent(packet.itprId, packet.vmId, make_shared<script::cEvent>(packet.eventName));
	m_protocol.AckInput(packet.senderId, true, packet.itprId, packet.vmId, 1);
	return true;
}


// request interpreter event protocol handler
bool cRemoteInterpreter::ReqEvent(remotedbg2::ReqEvent_Packet &packet) 
{ 
	// make event variables
	map<string, variant_t> vars;
	for (auto& kv : packet.values)
	{
		// trick code: one variable use
		const vector<string>& value = kv.second;
		if (!value.empty())
			vars.insert({ kv.first, value[0].c_str() });
	}
	//~

	PushEvent(packet.itprId, packet.vmId, make_shared<script::cEvent>(packet.eventName, vars));
	m_protocol.AckEvent(packet.senderId, true
		, packet.itprId, packet.vmId, packet.eventName, 1);
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


// remotedbg2 protocol, request debug info, sync instruction, symboltable, register
bool cRemoteInterpreter::ReqDebugInfo(remotedbg2::ReqDebugInfo_Packet &packet) 
{ 
	// default all vm network synchronize
	if (0)
	{
		// update network synchronize streaming
		set<int> syncVmIds;
		for (auto& vmId : packet.vmIds)
			syncVmIds.insert(vmId);
		set<int> curSyncVmIds; // current synchronize vm
		for (auto& itpr : m_interpreters)
			for (auto& vm : itpr.interpreter->m_vms)
				if (vm->m_nsync.enable)
				{
					curSyncVmIds.insert(vm->m_id);
					if (syncVmIds.end() == syncVmIds.find(vm->m_id))
						vm->EnableNetworkSync(false);
				}
		set<int> newSyncVmIds;
		for (auto& vmId : packet.vmIds)
			if (curSyncVmIds.end() == curSyncVmIds.find(vmId))
				newSyncVmIds.insert(vmId);
		for (auto& vmId : newSyncVmIds)
			if (script::cVirtualMachine* vm = GetVM(vmId))
				vm->EnableNetworkSync(true);
	}
	//~

	m_protocol.AckDebugInfo(packet.senderId, true, packet.vmIds, 1);
	return true;
}


// remotedbg2 protocol, request change variable
bool cRemoteInterpreter::ReqChangeVariable(remotedbg2::ReqChangeVariable_Packet& packet)
{
	map<string, variant_t> vars;
	script::sVariable* var = nullptr;
	vector<string> out;
	script::cVirtualMachine *vm = GetVM(packet.vmId);
	if (!vm)
		goto $error1;
	
	common::tokenizer(packet.varName, "::", "", out);
	if (out.size() < 2)
		goto $error1;

	var = vm->m_symbTable.FindVarInfo(out[0], out[1]);
	if (!var)
		goto $error1;

	// change symbol event
	vars[packet.varName] = common::str2variant(var->var.vt, packet.value);
	PushEvent(packet.itprId, packet.vmId, make_shared<script::cEvent>("@@symbol@@", vars));

	m_protocol.AckChangeVariable(packet.senderId, true
		, packet.itprId, packet.vmId, packet.varName, 1);
	return true;


$error1:
	m_protocol.AckChangeVariable(packet.senderId, true
		, packet.itprId, packet.vmId, packet.varName, 0);

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
	m_syncVMIds.clear();
	m_server.Close();
	ClearInterpreters();
	m_threads = nullptr;
	m_modules.clear();
}


namespace {
	common::cMemoryPool2<1024 * 100> g_memPool;
}

// send intermediate code, using remotedbg2 protocol
bool SendIntermediateCode(remotedbg2::h2r_Protocol &protocol
	, const netid recvId, const int itprId, const int vmId
	, const script::cIntermediateCode &icode)
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
		protocol.AckIntermediateCode(recvId, true, itprId, vmId, 0, 0, 0, 0, {});
	}
	else
	{
		// 12 bytes = itpr, vmId, result, count, index
		// 2 * 4byte = vmId, totalBufferSize, buffsize
		// 20 bytes = 12 + 8
		const uint CHUNK_SIZE = network2::DEFAULT_PACKETSIZE -
			(marsh.GetHeaderSize() + 20);
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
				, (BYTE)itprId, vmId, 1, (BYTE)totalCount, (BYTE)index, bufferSize, data);

			cursor += size;
			++index;
		}
	}

	g_memPool.Free(buff);
	return true;
}
