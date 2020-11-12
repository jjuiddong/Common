//
// 2019-01-08, jjuiddong
// protocol interface
//
// Defines the basic protocol interface of the sending and receiving packets.
//
// 2020-11-11
//	- add packet format type (ePacketFormat)
//
#pragma once


namespace network2
{
	class cNetworkNode;

	interface iProtocol
	{
		iProtocol(const int id, const ePacketFormat format = ePacketFormat::BINARY) 
			: m_id(id), m_format(format) {}
		virtual ~iProtocol() {}

		int GetId() const { return m_id; }
		cNetworkNode* GetNode() { return m_node; }
		void SetNode(cNetworkNode* node) { m_node = node; }

		int m_id; // protocol id, match cProtocolDispatcher::m_id
		ePacketFormat m_format; // packet data format
		cNetworkNode *m_node; // reference
	};

}
