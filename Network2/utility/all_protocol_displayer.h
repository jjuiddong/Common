//
// 2019-02-23, jjuiddong
// All Protocol Displayer
//	- Log or Print All Protocol Information
//
#pragma once


namespace network2
{

	class AllProtocolDisplayer : public all::ProtocolHandler
	{
	public:
		AllProtocolDisplayer() : m_logLevel(1) {}
		virtual void RecvPacket(cPacket &packet) override;

	protected:
		int m_logLevel; // default:1
		set<std::pair<int,int>> m_ignorePackets; // protocolid, packetid
		set<std::pair<int, int>> m_showPackets; // protocolid, packetid
	};

}
