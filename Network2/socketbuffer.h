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
		cSocketBuffer(const netid netId, const uint maxBufferSize);
		virtual ~cSocketBuffer();

		uint Push(iPacketHeader *packetHeader, const BYTE *data, const uint size);
		bool Pop(OUT cPacket &out);
		bool Pop(const uint size);
		bool PopNoRemove(OUT cPacket &out);
		bool IsEmpty();
		void Clear();


	public:
		netid m_netId; // session id
		uint m_totalLen; // packet total length
		uint m_readLen; // data read length
		bool m_isHeaderCopy;
		BYTE m_tempHeader[DEFAULT_PACKETSIZE * 2];
		common::cCircularQueue2<BYTE> m_q;
	};

}
