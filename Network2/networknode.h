//
// 2019-01-08, jjuiddong
// Network Base Class
//
// Defines the basic interface for network communication 
//
// 2021-09-08
//	- packet header refactoring
//
#pragma once


namespace network2
{
	interface iProtocol;

	class cNetworkNode : public cSession
	{
		friend class cPacketQueue;
	public:
		cNetworkNode(const StrId &name = "NetNode"
			, const int logId = -1
		);
		virtual ~cNetworkNode();

		void RegisterProtocol(iProtocol *protocol);
		virtual bool AddProtocolHandler(iProtocolHandler *handler);
		virtual bool RemoveProtocolHandler(iProtocolHandler *handler);
		vector<iProtocolHandler*>& GetProtocolHandlers();
		//bool CreatePacketHeader(const ePacketFormat format);
		//iPacketHeader* GetPacketHeader();
		virtual void Close() override;

		// pure virtual functions..
		virtual SOCKET GetSocket(const netid netId) = 0;
		virtual netid GetNetIdFromSocket(const SOCKET sock) = 0;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) = 0;
		virtual int Send(const netid rcvId, const cPacket &packet) = 0;
		virtual int SendAll(const cPacket &packet) = 0;


	protected:
		// default send, sendto implements
		virtual int SendPacket(const SOCKET sock, const cPacket &packet);
		virtual int SendToPacket(const SOCKET sock, const sockaddr_in &sockAddr
			, const cPacket &packet);


	public:
		//iPacketHeader *m_packetHeader;
		vector<iProtocolHandler*> m_protocolHandlers;
	};

}
