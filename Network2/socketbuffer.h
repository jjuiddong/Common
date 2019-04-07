//
// 2019-02-03, jjuiddong
// Socket Buffer
//	- Packet Queue specific socket
//	- Circular Queue
//
#pragma once


namespace network2
{

	class cSocketBuffer
	{
	public:
		cSocketBuffer(const netid netId);
		virtual ~cSocketBuffer();

		uint Push(iPacketHeader *packetHeader, const BYTE *data, const uint size);
		bool Pop(cPacket &out);
		void Clear();


	public:
		netid m_netId; // session id
		uint m_totalLen;
		uint m_readLen;
		bool m_isHeaderCopy;
		BYTE m_tempHeader[DEFAULT_PACKETSIZE * 2];
		common::cCircularQueue<BYTE, DEFAULT_SOCKETBUFFER_SIZE> m_q;
	};

}
