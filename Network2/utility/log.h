//
// 2019-03-08, jjuiddong
// network packet log defintion
//
#pragma once


namespace network2
{

	// Set Packet Log Directory Path
	void SetPacketLogPath(const int logId
		, const string &logFolderName, const string &subFolderName);

	// Get Packet Log Directory Path (use cSession class)
	const StrPath& GetPacketLogPath(const int logId);

	bool LogSession(const int logId, const cSession &session);
	bool LogPacket(const int logId, const netid sndId, const netid rcvId
		, const cPacket &packet);

}
