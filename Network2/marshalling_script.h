//
// 2020-11-21, jjuiddong
// script marshalling
//	- remotedbg2 protocol marshalling
//	- websocket protocol
//		- need binary, json marshalling
//
#pragma once


// json marshalling
namespace network2 {
	namespace marshalling_json
	{
		inline ptree& put(ptree &props, const char *typeName, const script::cIntermediateCode &rhs) { return props; } // nothing
		inline ptree& get(ptree &props, const char *typeName, OUT script::cIntermediateCode &rhs) { return props; } // nothing
		inline ptree& put(ptree &props, const char *typeName, const script::cVirtualMachine::sRegister &rhs) { return props; } // nothing
		inline ptree& get(ptree &props, const char *typeName, OUT script::cVirtualMachine::sRegister &rhs) { return props; } // nothing
		inline ptree& put(ptree &props, const char *typeName, const script::sSyncSymbol &rhs) { return props; } // nothing
		inline ptree& get(ptree &props, const char *typeName, OUT script::sSyncSymbol &rhs) { return props; } // nothing
		inline ptree& put(ptree &props, const char *typeName, const vector<script::sSyncSymbol> &rhs) { return props; } // nothing
		inline ptree& get(ptree &props, const char *typeName, OUT vector<script::sSyncSymbol> &rhs) { return props; } // nothing
	}
}


// binary marshalling
namespace network2 {
	namespace marshalling {

		//---------------------------------------------------------------------------------
		// <<
		inline cPacket& operator<<(cPacket& packet, const script::sInstruction &rhs)
		{
			packet << (BYTE)rhs.cmd;
			packet << rhs.str1;
			packet << rhs.str2;
			packet << rhs.str3;
			packet.Write4ByteAlign();
			packet << rhs.reg1;
			packet << rhs.reg2;
			packet << rhs.var1;
			return packet;
		}

		inline cPacket& operator<<(cPacket& packet, const script::cIntermediateCode &rhs)
		{
			packet << rhs.m_codes.size();
			for (auto &inst : rhs.m_codes)
				packet << inst;
			return packet;
		}

		inline cPacket& operator<<(cPacket& packet, const script::cVirtualMachine::sRegister &rhs)
		{
			packet << rhs.idx;
			packet << rhs.cmp;
			packet << rhs.val;
			//for (uint i = 0; i < ARRAYSIZE(rhs.val); ++i)
			//	packet << rhs.val[i];
			return packet;
		}

		inline cPacket& operator<<(cPacket& packet, const script::sSyncSymbol &rhs)
		{
			packet << *rhs.scope;
			packet << *rhs.name;
			packet << *rhs.var;
			return packet;
		}

		inline cPacket& operator<<(cPacket& packet, const vector<script::sSyncSymbol> &rhs)
		{
			MARSHALLING_BIN_APPEND_SEQ(packet, rhs);
			return packet;
		}

		//---------------------------------------------------------------------------------
		// >>
		inline cPacket& operator>>(cPacket& packet, OUT script::cIntermediateCode& rhs) { return packet; } // nothing~
		inline cPacket& operator>>(cPacket& packet, OUT script::cVirtualMachine::sRegister& rhs) { return packet; } // nothing
		inline cPacket& operator>>(cPacket& packet, OUT script::sSyncSymbol& rhs) { return packet; } // nothing
		inline cPacket& operator>>(cPacket& packet, OUT vector<script::sSyncSymbol>& rhs) { return packet; } // nothing
	}
}

