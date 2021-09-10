//
// 2019-01-08, jjuiddong
// Protocol Dispatcher
//	packet dispatcher 
//
// 네트워크로부터 받은 패킷을 해당하는 프로토콜에 맞게 분석해서 Handler를 호출한다.
// Dispatch 함수에서 개별 프로토콜 함수들을 호출하게 된다.
//
// Network Protocol Generation Compiler generates Dispatch () as a source file.
//
// 2020-11-11
//	- add packet format type (ePacketFormat)
//
// 2021-09-08
//	- add PacketHeaderMap
//
#pragma once


namespace network2
{
	
	class cProtocolDispatcher
	{
	public:
		cProtocolDispatcher(const int id, const ePacketFormat format = ePacketFormat::BINARY);
		virtual ~cProtocolDispatcher();

		int GetId() const { return m_id; }
		void SetCurrentDispatchPacket(cPacket *pPacket);
		virtual bool Dispatch(cPacket &packet, const vector<iProtocolHandler*> &handlers) = 0;


	public:
		int m_id; // dispatcher id, same as protocol id
		ePacketFormat m_format;
		cPacket *m_packet; // reference

		//-----------------------------------------------------------------
		// singleton DispatcherMap
		static map<int, cProtocolDispatcher*>* GetDispatcherMap() {
			if (!s_dispatchers)
				s_dispatchers = new map<int, cProtocolDispatcher*>();
			return s_dispatchers;
		}
		static void ReleaseDispatcherMap() {
			SAFE_DELETE(s_dispatchers);
		}
		static map<int, cProtocolDispatcher*> *s_dispatchers; //key: protocol id

		//-----------------------------------------------------------------
		// singleton PacketHeaderMap
		static map<int, iPacketHeader*>* GetPacketHeaderMap() {
			if (!s_packetHeaders)
				s_packetHeaders = new map<int, iPacketHeader*>();
			return s_packetHeaders;
		}
		static void ReleasePacketHeaderMap() {
			SAFE_DELETE(s_packetHeaders);
		}
		static map<int, iPacketHeader*> *s_packetHeaders; // key: protocol id
	};

}
