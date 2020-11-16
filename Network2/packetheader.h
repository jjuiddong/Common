//
// 2019-01-08, jjuiddong
// packet header class
//
// generalizes the packet header information.
//
//
// Default PacketHeader : cPacketHeader (binary format)
//
// --------------------------------------------------------------------
// |              cPacketHeader : Protocol Header                     |
// --------------------------------------------------------------------
// | protocol ID (4byte) | packet ID (4byte) | packet Length (4byte)  |
// --------------------------------------------------------------------
//
#pragma once


namespace network2
{

	interface iPacketHeader
	{
		virtual bool IsNoFormat() = 0;
		virtual uint GetHeaderSize() = 0;
		virtual int GetProtocolId(const BYTE *src) = 0;
		virtual uint GetPacketId(const BYTE *src) = 0;
		virtual uint GetPacketLength(const BYTE *src) = 0;
		virtual uint GetOptionBits(const BYTE *src, const uint mask) = 0;
		virtual void SetProtocolId(BYTE *dst, const int protocolId) = 0;
		virtual void SetPacketId(BYTE *dst, const uint packetId) = 0;
		virtual void SetPacketLength(BYTE *dst, const uint packetLength) = 0;
		virtual void SetOptionBits(BYTE *dst, const uint mask, const uint options) = 0;
		virtual int SetPacketTerminator(BYTE *dst, const int remainSize) = 0;
		virtual int SetDelimeter(BYTE *dst) = 0;
		virtual bool IsValidPacket(const BYTE *src) = 0;

	};


	// Binary Packet Header
	class cPacketHeader : public iPacketHeader
	{
	public:
		virtual bool IsNoFormat() override { return false; }
		virtual uint GetHeaderSize() override;
		virtual int GetProtocolId(const BYTE *src) override;
		virtual uint GetPacketId(const BYTE *src) override;
		virtual uint GetPacketLength(const BYTE *src) override;
		virtual uint GetOptionBits(const BYTE *src, const uint mask) override { return 0; } // nothing
		virtual void SetProtocolId(BYTE *dst, const int protocolId) override;
		virtual void SetPacketId(BYTE *dst, const uint packetId) override;
		virtual void SetPacketLength(BYTE *dst, const uint packetLength) override;
		virtual int SetPacketTerminator(BYTE *dst, const int remainSize) override { return 0; } // nothing
		virtual int SetDelimeter(BYTE *dst) override { return 0; } // nothing
		virtual void SetOptionBits(BYTE *dst, const uint mask, const uint options) override {} // nothing
		virtual bool IsValidPacket(const BYTE *src) override;
	};


	// No Format Packet Header
	class cPacketHeaderNoFormat : public iPacketHeader
	{
	public:
		virtual bool IsNoFormat() override { return true; }
		virtual uint GetHeaderSize() override { return 0; }
		virtual int GetProtocolId(const BYTE *src) override { return 0; }
		virtual uint GetPacketId(const BYTE *src) override { return 0; }
		virtual uint GetPacketLength(const BYTE *src) override { return 0; }
		virtual uint GetOptionBits(const BYTE *src, const uint mask) override { return 0; } // nothing
		virtual void SetProtocolId(BYTE *dst, const int protocolId) override {}
		virtual void SetPacketId(BYTE *dst, const uint packetId) override {}
		virtual void SetPacketLength(BYTE *dst, const uint packetLength) override {}
		virtual int SetPacketTerminator(BYTE *dst, const int remainSize) override { return 0; } // nothing
		virtual int SetDelimeter(BYTE *dst) override { return 0; }; // nothing
		virtual void SetOptionBits(BYTE *dst, const uint mask, const uint options) override {} // nothing
		virtual bool IsValidPacket(const BYTE *src) override { return true; }
	};

}
