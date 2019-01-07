//
// 2019-01-04, jjuiddong
// packet protocol header class interface
//
// cProtocol
//		protocol type : 4 byte ascii type
//		packet length : 4 byte ascii type
//
//		header {
//			char protocol[4];
//			char packetSize[4];
//		};
//
#pragma once


namespace network
{

	interface iProtocol 
	{
		virtual bool MakeHeader(const char *protocol, const int length) = 0;
		virtual bool GetHeader(const BYTE *data, OUT int &byteSize) = 0;
		virtual bool WriteHeader(BYTE *dst, const int packetSize) = 0;
		virtual int GetHeaderSize() = 0;
		virtual int GetProtocolType() = 0;
		virtual int GetPacketLength() = 0;
		virtual bool IsValidPacket() = 0;
	};


	class cProtocol : public iProtocol
	{
	public:
		cProtocol();
		cProtocol(const char protocol[4]);
		virtual ~cProtocol();

		virtual bool MakeHeader(const char *protocol, const int length) override;
		virtual bool GetHeader(const BYTE *data, OUT int &byteSize) override;
		virtual bool WriteHeader(BYTE *dst, const int packetSize) override;
		virtual int GetHeaderSize() override;
		virtual int GetProtocolType() override;
		virtual int GetPacketLength() override;
		virtual bool IsValidPacket() override;

	public:
		char m_protocol[4];
		int m_packetLength;
	};

}
