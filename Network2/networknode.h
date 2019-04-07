//
// 2019-01-08, jjuiddong
// Network Base Class
//
// Defines the basic interface for network communication 
//
#pragma once


namespace network2
{
	interface iProtocol;

	class cNetworkNode : public cSession
	{
	public:
		cNetworkNode(iPacketHeader *packetHeader
			, const StrId &name = "NetNode"
			, const bool isPacketLog = false
		);
		virtual ~cNetworkNode();

		void RegisterProtocol(iProtocol *protocol);
		virtual bool AddProtocolHandler(iProtocolHandler *handler);
		virtual bool RemoveProtocolHandler(iProtocolHandler *handler);
		vector<iProtocolHandler*>& GetProtocolHandlers();
		iPacketHeader* GetPacketHeader();
		virtual void Close() override;

		// child implements
		virtual SOCKET GetSocket(const netid netId) = 0;
		virtual netid GetNetIdFromSocket(const SOCKET sock) = 0;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) = 0;
		virtual int Send(const netid rcvId, const cPacket &packet) = 0;
		virtual int SendAll(const cPacket &packet) = 0;


	public:
		iPacketHeader *m_packetHeader;
		vector<iProtocolHandler*> m_protocolHandlers;
	};

}
