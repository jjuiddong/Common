//
// 2019-01-08, jjuiddong
// protocol interface
//
// Defines the basic protocol interface of the sending and receiving packets.
//
#pragma once


namespace network2
{
	class cNetworkNode;

	interface iProtocol
	{
		iProtocol(const int id) : m_id(id) {}
		virtual ~iProtocol() {}
		int GetId() const { return m_id; }
		cNetworkNode* GetNode() { return m_node; }
		void SetNode(cNetworkNode* node) { m_node = node; }

		int m_id; // match cProtocolDispatcher::m_id
		cNetworkNode *m_node; // reference
	};

}
