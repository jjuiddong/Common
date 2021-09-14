//
// 2019-01-09, jjuiddong
//
// PacketQueue class
// packet save . Thread Safe
// - 한 패킷이 분리되어서 네트워크로 들어올 때, 합치는 기능이 있다. (cSocketBuffer에서 처리)
// - 각 소켓마다 packetSize 크기만큼 채울 때까지 다음으로 넘어가지 않는다.
// - 버퍼가 가득차면 들어온 패킷은 무시한다.
//
// 2019-02-03
//	- cSocketBuffer 추가
//
// 2019-03-08
//	- Packet Log
//		- SendAll(), send packet log
//		- Front(), recv packet log
//		- Log Data
//			- sender id, receiver id
//			- packet data
//
// 2020-11-12
//	- send, sendto interface with cNetNode
//
#pragma once


namespace network2
{
	
	class cSession;
	class cPacketQueue
	{
	public:
		cPacketQueue(cNetworkNode *node, const int logId = -1);
		virtual ~cPacketQueue();

		bool Init(const int packetSize, const int maxPacketCount);
		bool Push(const netid rcvId, const cPacket &packet);
		bool Push(const netid senderId, const BYTE *data, const int len);
		bool Push(const netid senderId, iPacketHeader *packetHeader
			, const BYTE *data, const int len);
		bool Front(OUT cPacket &out);
		void SendAll(const map<netid, SOCKET> &socks, OUT set<netid> *outErrSocks = NULL);
		void SendAll(const sockaddr_in &sockAddr);
		bool SendBroadcast(const vector<cSession*> &sessions, const bool exceptOwner = true);
		bool SendBroadcast(const map<netid, SOCKET> &socks, const cPacket &packet
			, OUT set<netid> *outErrSocks = NULL);
		void Lock();
		void Unlock();
		void Clear();


	public:
		int m_logId; // packet log id, default: -1
		int m_sockBufferSize; // packetSize * maxPacketCount
		cNetworkNode *m_netNode; // owner networknode, reference
		common::VectorMap<netid, cSocketBuffer*> m_sockBuffers;
		int m_nextFrontIdx; // for Front() load balancing
		bool m_isLogIgnorePacket; // log queue full?, default = false
		CRITICAL_SECTION m_cs;
	};

}
