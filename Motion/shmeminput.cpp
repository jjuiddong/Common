
#include "stdafx.h"
#include "shmeminput.h"


using namespace motion;


cShmemInput::cShmemInput() 
	: cInput(MOTION_MEDIA::SHARED_MEM)
	, m_memPtr(NULL)
	, m_startIndex(1001)
	, m_memorySize(1024)
{
}

cShmemInput::~cShmemInput()
{
}


// 클래스 초기화
bool cShmemInput::Init(const int startIndex, const int memSize, const string &sharedMemoryName, const bool isBigEndian,
	const string &protocolCmd, const string &cmd, const string &modulatorScript)
{
	m_startIndex = startIndex;
	m_memorySize = memSize;
	m_isBigEndian = isBigEndian;

	if (!CreateSharedMem(sharedMemoryName))
		return false;

	if (!m_protocolParser.ParseStr(protocolCmd, "\n"))
		return false;

	if (!m_cmdParser.ParseStr(cmd))
		return false;

	m_modulator.ParseStr(modulatorScript);

	return true;
}


bool cShmemInput::Init2(const int startIndex, const int memSize, const string &sharedMemoryName, const bool isBigEndian,
	const string &protocolScriptFileName,const string &cmdScriptFileName, const string &modulatorScriptFileName)
{
	m_startIndex = startIndex;
	m_memorySize = memSize;
	m_isBigEndian = isBigEndian;

	if (!CreateSharedMem(sharedMemoryName))
		return false;

	if (!m_protocolParser.Read(protocolScriptFileName, "\n"))
		return false;

	if (!m_cmdParser.Read(cmdScriptFileName))
		return false;

	if (!m_modulator.Read(modulatorScriptFileName))
		return false;

	return true;
}


// 공유메모리 생성
bool cShmemInput::CreateSharedMem(const string &sharedMemoryName)
{
	using namespace boost::interprocess;

	try
	{
		m_sharedmem = windows_shared_memory(open_or_create, sharedMemoryName.c_str(), read_write, m_memorySize);

		m_mmap = mapped_region(m_sharedmem, read_write, 0, m_memorySize);

		m_memPtr = static_cast<BYTE*>(m_mmap.get_address());
	}
	catch (interprocess_exception&e)
	{
		std::stringstream ss;
		ss << e.what() << std::endl;
		ss << "Shared Memory Error!!";
		//::AfxMessageBox(str2wstr(ss.str()).c_str());
		dbg::ErrLog(ss.str().c_str());
		return false;
	}

	return true;
}


bool cShmemInput::Start()
{
	m_state = MODULE_STATE::START;

	return true;
}


bool cShmemInput::Stop()
{
	m_state = MODULE_STATE::STOP;

	return true;
}


bool cShmemInput::Update(const float deltaSeconds)
{
	RETV(m_state == MODULE_STATE::STOP, false);
	RETV(m_memPtr==NULL, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	ParseSharedMem(m_memPtr, m_memorySize);

	// Excute Command Script
	m_cmdInterpreter.Excute(m_cmdParser.m_stmt);

	// Calc Modulation
	const float roll = script::g_symbols["@roll"].fVal;
	const float pitch = script::g_symbols["@pitch"].fVal;
	const float yaw = script::g_symbols["@yaw"].fVal;
	const float heave = script::g_symbols["@heave"].fVal;
	m_modulator.Update(m_incTime, yaw, pitch, roll, heave);

	// Save Symboltable Modulation Output Data
	float froll, fpitch, fyaw, fheave;
	m_modulator.GetFinal(fyaw, fpitch, froll, fheave);

	script::sFieldData data;
	data.type = script::FIELD_TYPE::T_FLOAT;
	data.fVal = froll;
	script::g_symbols["@roll_mod"] = data;
	data.fVal = fpitch;
	script::g_symbols["@pitch_mod"] = data;
	data.fVal = fyaw;
	script::g_symbols["@yaw_mod"] = data;
	data.fVal = fheave;
	script::g_symbols["@heave_mod"] = data;

	m_incTime = 0;

	return true;
}


// 공유메모리 분석
// 공유메모리 정보는 $1001 부터 시작한다.
void cShmemInput::ParseSharedMem(const BYTE *buffer, const int bufferLen)
{
	int i = 0;
	int index = 0;
	const BYTE *pmem = buffer;
	for each (auto &field in m_protocolParser.m_fields)
	{
		if (index > bufferLen)
			break;

		script::sFieldData data;
		ZeroMemory(data.buff, sizeof(data));
		memcpy(data.buff, pmem, min(field.bytes, sizeof(data.buff)));
		data.type = field.type;

		if (m_isBigEndian)
			script::bigEndian((BYTE*)data.buff, min(field.bytes, sizeof(data.buff)));

		const string id = format("$%d", i + m_startIndex); // $1 ,$2, $3 ~
		const string oldId = format("@%d", i + m_startIndex); // @1, @2, @3 ~

		script::g_symbols[oldId] = script::g_symbols[id]; // 전 데이타를 저장한다. 
		script::g_symbols[id] = data;

		index += field.bytes;
		pmem += field.bytes;
		++i;
	}
}
