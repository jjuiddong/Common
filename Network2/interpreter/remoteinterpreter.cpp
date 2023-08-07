
#include "stdafx.h"
#include "remoteinterpreter.h"

using namespace network2;

namespace
{
	const float TIME_SYNC_INSTRUCTION = 0.5f; // seconds unit
	const float TIME_SYNC_REGISTER = 5.0f; // seconds unit
	const float TIME_SYNC_SYMBOL = 3.0f; // seconds unit
}

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
			goto $error;		
		interpreter = m_rmtItpr->m_interpreters[m_itprId].interpreter;
		vm = interpreter->GetVM(m_vmId);
		if (!vm)
			goto $error;		

		SendIntermediateCode(m_rmtItpr->m_protocol, m_rcvId, m_itprId, m_vmId
			, vm->m_code);
		--m_rmtItpr->m_multiThreading;
		return eRunResult::End;


	$error:
		// fail, iterpreter id invalid
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
bool cRemoteInterpreter::LoadIntermediateCode(const StrPath &fileName)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init();

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
// vmName: virtual machine name
bool cRemoteInterpreter::LoadIntermediateCode(
	const common::script::cIntermediateCode &icode)
{
	script::cInterpreter *interpreter = new script::cInterpreter();
	interpreter->Init();

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
		itpr.regSyncTime = TIME_SYNC_REGISTER + 1.0f;
		itpr.instSyncTime = TIME_SYNC_INSTRUCTION + 1.0f;
		itpr.symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;
		SendSyncInstruction(itprId);
		SendSyncVMRegister(itprId);
		SendSyncSymbolTable(itprId, -1);
	}
	return true;
}


// process webclient, interpreter
bool cRemoteInterpreter::Process(const float deltaSeconds)
{
	// sync instruction, register, symboltable
	// check change instruction
	for (uint k=0; k < m_interpreters.size(); ++k)
	{
		const int itprId = (int)k;
		sItpr &itpr = m_interpreters[itprId];
		itpr.interpreter->Process(deltaSeconds);

		if (itpr.state != eState::Run)
			continue;
		//const bool isSync = (m_syncItptrs.end() != m_syncItptrs.find(itprId));
		set<int> syncVms; // sync vm id

		itpr.regSyncTime += deltaSeconds;
		itpr.instSyncTime += deltaSeconds;
		itpr.symbSyncTime += deltaSeconds;

		script::cInterpreter *interpreter = itpr.interpreter;
		for (uint i = 0; i < interpreter->m_vms.size(); ++i)
		{
			script::cVirtualMachine *vm = interpreter->m_vms[i];

			if (m_syncVMIds.end() != m_syncVMIds.find(vm->m_id))
				syncVms.insert(vm->m_id);

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
			if (!syncVms.empty())
				SendSyncVMRegister(itprId, &syncVms);
		}

		// sync instruction?
		if (itpr.instSyncTime > TIME_SYNC_INSTRUCTION)
			SendSyncInstruction(itprId);

		SendSyncSymbolTable(itprId, -1);

		// is meet breakpoint? change step debugging mode
		if (interpreter->IsBreak())
		{
			m_protocol.AckOneStep(network2::ALL_NETID, true, itprId, 1);
		}
	}//~for interpreters

	return m_server.IsFailConnection() ? false : true;
}


// push interpreter event, wrapping function
// itprId: interpreter id, -1: all interpreter, all vm
// vmId: virtual machine id, -1: all vm
// return: success push
bool cRemoteInterpreter::PushEvent(const int itprId
	, const int vmId, const common::script::cEvent &evt
)
{
	bool res = false;
	if (itprId < 0)
	{
		for (auto &itpr : m_interpreters)
		{
			script::cInterpreter *interpreter = itpr.interpreter;
			if (eState::Run == itpr.state)
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
bool cRemoteInterpreter::Run(const int itprId)
{
	return RunInterpreter(itprId, 0);
}


// start interpreter with debug run
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::DebugRun(const int itprId)
{
	return RunInterpreter(itprId, 1);
}


// start interpreter with one step debugging
// itprId: interpreter index, -1: all interpreter
bool cRemoteInterpreter::StepRun(const int itprId)
{
	return RunInterpreter(itprId, 2);
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
// vmId: -1: all vm
bool cRemoteInterpreter::SyncInformation(const int itprId, const int vmId)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;
	// sync all
	sItpr& itpr = m_interpreters[itprId];
	itpr.symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;
	SendSyncSymbolTable(itprId, vmId);
	return true;
}


// synchronize virtual machine information (only symboltable)
// vmId: virtual machine id
// varNames: 'scopeName;varName' array
bool cRemoteInterpreter::SyncVMInformation(const int vmId, const vector<string>& varNames)
{
	const int itprId = GetInterpreterIdByVMId(vmId);
	if (itprId < 0)
		return false; // error return

	sItpr& itpr = m_interpreters[itprId];
	if (IsChangeSymbolTable(itprId))
	{
		// sync all
		itpr.symbSyncTime = TIME_SYNC_SYMBOL + 1.0f;
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


// find interpreter id (index)
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


// find interpreter
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
// type: 0:run, 1:debug run, 2:step run
bool cRemoteInterpreter::RunInterpreter(const int itprId, const int type)
{
	if (itprId < 0)
	{
		for (uint i = 0; i < m_interpreters.size(); ++i)
			RunInterpreter_Sub(i, type);
	}
	else
	{
		return RunInterpreter_Sub(itprId, type);
	}
	return true;
}


// run interpreter
// itprId: interpreter id, -1:all interpreter
// type: 0:run, 1:debug run, 2:step run
bool cRemoteInterpreter::RunInterpreter_Sub(const int itprId, const int type)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;

	bool result = false;
	sItpr& itpr = m_interpreters[itprId];
	if (eState::Stop == itpr.state)
	{
		script::cInterpreter* interpreter = itpr.interpreter;
		interpreter->SetCodeTrace(true);
		for (auto& mod : m_modules)
			interpreter->AddModule(mod);

		switch (type)
		{
		case 0: // Run
			if (result = interpreter->Run())
			{
				itpr.state = eState::Run;
				SyncInformation(itprId, -1);
				return true;
			}
			break;
		case 1: // DebugRun
			if (result = interpreter->DebugRun())
			{
				itpr.state = eState::Run;
				SyncInformation(itprId, -1);
			}
			break;
		case 2: // StepRun
			if (result = interpreter->StepRun())
				itpr.state = eState::Run;
			break;
		default:
			return false;
		}
	}
	return result;
}


// check symboltable change
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


// sync instruction set
bool cRemoteInterpreter::SendSyncInstruction(const int itprId)
{
	sItpr &itpr = m_interpreters[itprId];
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
			, true, itprId, vm->m_id, vm->m_trace);

		vm->ClearCodeTrace(true);
	}
	return true;
}


// sync vm register info
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncVMRegister(const int itprId
	, const set<int> *vmIds //=nullptr
)
{
	if (m_interpreters.size() <= (uint)itprId)
		return false;
	script::cInterpreter *interpreter = m_interpreters[itprId].interpreter;
	for (uint i=0; i < interpreter->m_vms.size(); ++i)
	{
		auto &vm = interpreter->m_vms[i];

		if (vmIds)
			if (vmIds->end() == vmIds->find(vm->m_id))
				continue; // ignore this vm

		m_protocol.SyncVMRegister(network2::ALL_NETID, true, itprId
			, vm->m_id, 0, vm->m_reg);
		break; // now only one virtual machine sync
	}
	return true;
}


// synchronize symboltable
// itprId: interpreter index
bool cRemoteInterpreter::SendSyncSymbolTable(const int itprId, const int vmId)
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
		if ((vmId >= 0) && (vm->m_id != vmId))
			continue; // ignore this vm

		for (auto &kv1 : vm->m_symbTable.m_vars)
		{
			const string &scopeName = kv1.first;
			for (auto &kv2 : kv1.second)
			{
				const string &name = kv2.first;
				const string varName = scopeName + "_" + kv2.first;
				const script::sVariable &var = kv2.second;

				bool isSync = false;
				auto it = itpr.chSymbols.find(varName);
				if (itpr.chSymbols.end() == it)
				{
					// add new symbol
					isSync = true;
					sSymbol ssymb;
					ssymb.name = varName;
					ssymb.t = 0;
					ssymb.var = var;
					itpr.chSymbols[varName] = ssymb;
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
	script::cEvent evt(packet.eventName);
	PushEvent(packet.itprId, packet.vmId, evt);
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

	script::cEvent evt(packet.eventName, vars);
	PushEvent(packet.itprId, packet.vmId, evt);
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
	m_syncVMIds.clear();
	for (auto &id : packet.vmIds)
		m_syncVMIds.insert(id);

	m_protocol.AckDebugInfo(packet.senderId, true, packet.vmIds, 1);
	return true;
}


// remotedbg2 protocol, request change variable
bool cRemoteInterpreter::ReqChangeVariable(remotedbg2::ReqChangeVariable_Packet& packet)
{
	// change symbol event
	map<string, variant_t> vars;
	vars[packet.varName] = packet.value.c_str();
	script::cEvent evt("@@symbol@@", vars);
	PushEvent(packet.itprId, packet.vmId, evt);
	//~

	m_protocol.AckChangeVariable(packet.senderId, true
		, packet.itprId, packet.vmId, packet.varName, 1);
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
	//m_chSymbols.clear();
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
