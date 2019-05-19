//
// 2019-01-09, jjuiddong
//
// ��Ŷ�� �����Ѵ�. Thread Safe
// - �� ��Ŷ�� �и��Ǿ ��Ʈ��ũ�� ���� ��, ��ġ�� ����� �ִ�.
// - �� ���ϸ��� packetSize ũ�⸸ŭ ä�� ������ �������� �Ѿ�� �ʴ´�.
// - ���۰� �������� ���� ��Ŷ�� �����Ѵ�.
//
// 2019-02-03
//	- cSocketBuffer �߰�
//
// 2019-03-08
//	- Packet Log
//		- SendAll(), send packet log
//		- Front(), recv packet log
//		- Log Data
//			- sender id, receiver id
//			- packet data
//
#pragma once


namespace network2
{
	
	class cSession;
	class cPacketQueue
	{
	public:
		cPacketQueue(cNetworkNode *node, const bool isPacketLog = false);
		virtual ~cPacketQueue();

		bool Init(const int packetSize, const int maxPacketCount);
		bool Push(const netid rcvId, const cPacket &packet);
		bool PushFromNetwork(const netid senderId, const BYTE *data, const int len);
		bool Front(OUT cPacket &out);
		void SendAll(const map<netid, SOCKET> &socks, OUT set<netid> *outErrSocks = NULL);
		void SendAll(const sockaddr_in &sockAddr);
		void SendBroadcast(const vector<cSession*> &sessions, const bool exceptOwner = true);
		void SendBroadcast(const map<netid, SOCKET> &socks, const cPacket &packet
			, OUT set<netid> *outErrSocks = NULL);
		void Lock();
		void Unlock();
		void Clear();


	public:
		bool m_isPacketLog;
		cNetworkNode *m_netNode; // owner networknode, reference
		common::VectorMap<netid, cSocketBuffer*> m_sockBuffers;
		int m_nextFrontIdx; // for Front() load balancing
		bool m_isLogIgnorePacket; // ������ ��Ŷ�� ���� ���, �α׸� ������ ����, default = false
		CRITICAL_SECTION m_cs;
	};

}