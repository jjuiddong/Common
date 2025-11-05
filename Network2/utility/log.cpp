
#include "stdafx.h"
#include "log.h"
#include <direct.h>


namespace
{
	map<int, string> g_packetLogPathMap; // log directory path map
	common::CriticalSection g_cs; // g_packetLogPathMap sync
	common::cWQSemaphore g_logThread; // Packet Log Thread
}

using namespace network2;


//------------------------------------------------------------------------
// Packet Log Thread Task
class cPacketLogTask : public cTask
//					, public common::cMemoryPool4<cPacketLogTask>
{
public:
	cPacketLogTask() : cTask(0, "cPacketLogTask") {}
	cPacketLogTask(const sPacketLogData &logData)
		: cTask(0, "cPacketLogTask"), m_logData(logData) {
	}
	virtual ~cPacketLogTask() {
		SAFE_DELETEA(m_logData.data);
	}

	virtual eRunResult Run(const double deltaSeconds) override
	{
		RETV(m_logData.id < 0, eRunResult::End);

		switch (m_logData.type)
		{
		case 0: // session log
		{
			const StrPath path = GetPacketLogPath(m_logData.id) + "sessions.ses";
			std::ofstream ofs(path.c_str(), std::ios::app);
			if (!ofs.is_open())
			{
				assert(0);
				break;
			}

			ofs << (char*)m_logData.data << std::endl;
		}
		break;
		case 1: // packet log
		{
			StrPath path;
			path.Format("%s%d.plog", GetPacketLogPath(m_logData.id).c_str()
				, m_logData.rcvId);
			cPacketLog::Write(path.c_str(), m_logData);
		}
		break;
		default: assert(0); break;
		}
		return eRunResult::End;
	}


public:
	sPacketLogData m_logData;
};
//-----------------------------------------------------------------------


// set packet log directory path
// total path: logFolderName + '/' + subFolderName + '/'
void network2::SetPacketLogPath(const int logId
	, const string &logFolderName, const string &subFolderName)
{
	AutoCSLock cs(g_cs);
	_mkdir(logFolderName.c_str());
	const string path = logFolderName + "/" + subFolderName + "/";
	_mkdir(path.c_str());
	g_packetLogPathMap[logId] = path;
}


// return packet log path by logId
// you must set packet log directory path using 'SetPacketLogPath()'
// ex) ./log_packet/yyyymmddhhmmssmmm/
const string& network2::GetPacketLogPath(const int logId)
{
	AutoCSLock cs(g_cs);
	static string emptyPath;
	auto it = g_packetLogPathMap.find(logId);
	if (g_packetLogPathMap.end() == it)
	{
		assert(0);
		return emptyPath;
	}
	else
	{
		return it->second;
	}
}


// write file with session data
// logId: packet log directory path id
bool network2::LogSession(const int logId, const cSession &session)
{
	if (logId < 0)
		return false; // disable log

	cPacketLogTask *task = new cPacketLogTask();
	task->m_logData.id = logId;
	task->m_logData.type = 0;
	const uint bufferSize = sizeof(session.m_name);
	task->m_logData.data = new BYTE[bufferSize];
	task->m_logData.size = bufferSize;
	
	if (session.m_name.empty())
	{
		sprintf_s((char*)task->m_logData.data, bufferSize
			, "%s:%d %d", session.m_ip.c_str(), session.m_port, session.m_id);
	}
	else
	{
		sprintf_s((char*)task->m_logData.data, bufferSize
			, "%s %d", session.m_name.c_str(), session.m_id);
	}

	g_logThread.PushTask(task);

	return true;
}


// write packet data to log file
// read by network2::cPacketLog
// logId: packet log directory path id
bool network2::LogPacket(const int logId, const netid sndId
	, const netid rcvId, const cPacket &packet)
{
	const uint packetSize = packet.GetPacketSize();
	if (0 == packetSize)
		return true; // nothing to do

	cPacketLogTask *task = new cPacketLogTask();
	task->m_logData.id = logId;
	task->m_logData.type = 1;
	task->m_logData.sndId = sndId;// packet.GetSenderId();
	task->m_logData.rcvId = rcvId;
	task->m_logData.data = new BYTE[packetSize];
	memcpy(task->m_logData.data, packet.m_data, packetSize);
	task->m_logData.size = packetSize;
	
	g_logThread.PushTask(task);
	return true;
}


// return packet log thread
cWQSemaphore& network2::GetLogThread()
{
	return g_logThread;
}
