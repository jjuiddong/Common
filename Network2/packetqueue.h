//
// 2019-01-09, jjuiddong
//
// PacketQueue class
// packet save. Thread Safe
//	- merge split packet data
//	- ignore packet if full buffer
//
// 2019-02-03
//	- add cSocketBuffer
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
// 2021-09-20
//	- socket buffer reuse
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
		void SendAll(const map<netid, SOCKET> &socks
			, OUT set<netid> *outErrs = nullptr
			, vector<iProtocolHandler*>* listeners = nullptr);
		void SendAll(set<netid> *outErrs = nullptr
			, vector<iProtocolHandler*>* listeners = nullptr);
		void SendAll(const sockaddr_in &sockAddr
			, vector<iProtocolHandler*>* listeners = nullptr);
		bool SendBroadcast(const vector<cSession*> &sessions, const bool exceptOwner = true);
		bool SendBroadcast(const map<netid, SOCKET> &socks, const cPacket &packet
			, OUT set<netid> *outErrSocks = nullptr);
		bool Remove(const netid id);
		void SetLogId(const int logId);
		void Lock();
		void Unlock();
		void ClearBuffer();
		void Clear();


	public:
		int m_logId; // packet log id, default: -1
		int m_sockBufferSize; // packetSize * maxPacketCount
		cNetworkNode *m_netNode; // owner networknode, reference
		common::VectorMap<netid, cSocketBuffer*> m_sockBuffers;
		set<cSocketBuffer*> m_frees; // reuse socket buffer
		int m_nextFrontIdx; // for Front() load balancing
		bool m_isLogIgnorePacket; // log queue full?, default = false
		CRITICAL_SECTION m_cs;
	};

}
