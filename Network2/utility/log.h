//
// 2019-03-08, jjuiddong
// network packet log defintion
//
#pragma once


namespace network2
{

	// Set Packet Log Directory Path
	void SetPacketLogPath(const string &logFolderName, const string &subFolderName);


	// Get Packet Log Directory Path (use cSession class)
	const StrPath& GetPacketLogPath();


	bool LogSession(const cSession &session);
	bool LogPacket(const netid rcvId, const cPacket &packet);

}
