//
// 2019-01-08, jjuiddong
// ASCII packet header class
//
// generalizes the packet header information.
// ASCII Format Packet Header
//
// -------------------------------------------------------------
// |     Protocol ID    |     Packet ID     | Packet Size     |
// |      0 bytes       |   4 bytes (ASCII) |  4 bytes (ASCII)|
// -------------------------------------------------------------
//
// HeaderSize Total 8 Bytes
//
#pragma once


namespace network2
{

	class cPacketHeaderAscii : public iPacketHeader
	{
	public:
		cPacketHeaderAscii(const int protocolId = 100);
		virtual bool IsNoFormat() override { return false; }
		virtual uint GetHeaderSize() override;
		virtual int GetProtocolId(const BYTE *src) override;
		virtual uint GetPacketId(const BYTE *src) override;
		virtual uint GetPacketLength(const BYTE *src) override;
		virtual uint GetOptionBits(const BYTE *src, const uint mask) override { return 0; } // nothing
		virtual void SetProtocolId(BYTE *dst, const int protocolId) override;
		virtual void SetPacketId(BYTE *dst, const uint packetId) override;
		virtual void SetPacketLength(BYTE *dst, const uint packetLength) override;
		virtual void SetOptionBits(BYTE *dst, const uint mask, const uint options) override {} // nothing
		virtual int SetPacketTerminator(BYTE *dst, const int remainSize) override;
		virtual int SetDelimeter(BYTE *dst) override;
		virtual bool IsValidPacket(const BYTE *src) override;


	public:
		const int m_protocolId;
	};

}
