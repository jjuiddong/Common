//
// 2019-02-23, jjuiddong
// all protocol handler
//
#pragma once


namespace all
{
	using namespace network2;
	static const int Dispatcher_ID = -1;

	// All Protocol Dispatcher
	class Dispatcher : public network2::cProtocolDispatcher
	{
	public:
		Dispatcher() : cProtocolDispatcher(all::Dispatcher_ID) {}
		virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	};


	// All Protocol Handler
	class ProtocolHandler : virtual public network2::iProtocolHandler
	{
		friend class Dispatcher;

		// call handling packet
		virtual void RecvPacket(cPacket &packet) {}
	};

}
