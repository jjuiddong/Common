//
// 2020-11-16, jjuiddong
// json packet header class
//		- json packet consist json string or binary
//		- determine whether it is json or binary with the option bit
//
// Packet Header Format
//
// -------------------------------------------------------------------------
// | Protocol ID | Packet ID | Packet Size | Option (Binary Format Check) |
// |   4 bytes   |  4 bytes	 |   4 bytes   |        4 bytes	              |
// -------------------------------------------------------------------------
//
// HeaderSize Total 16 Bytes
//
#pragma once


namespace network2
{

	class cPacketHeaderJson : public iPacketHeader
	{
	public:
		virtual bool IsNoFormat() override { return false; }
		virtual uint GetHeaderSize() override;
		virtual int GetProtocolId(const BYTE *src) override;
		virtual uint GetPacketId(const BYTE *src) override;
		virtual uint GetPacketLength(const BYTE *src) override;
		virtual uint GetOptionBits(const BYTE *src, const uint mask) override;
		virtual void SetProtocolId(BYTE *dst, const int protocolId) override;
		virtual void SetPacketId(BYTE *dst, const uint packetId) override;
		virtual void SetPacketLength(BYTE *dst, const uint packetLength) override;
		virtual int SetPacketTerminator(BYTE *dst, const int remainSize) override { return 0; } // nothing
		virtual int SetDelimeter(BYTE *dst) override { return 0; } // nothing
		virtual void SetOptionBits(BYTE *dst, const uint mask, const uint options) override;
		virtual bool IsValidPacket(const BYTE *src) override;
	};

}
