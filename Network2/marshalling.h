//
// 2019-08-20, jjuiddong
// Marshalling common
//
#pragma once


namespace network2
{

	// enum Marshalling Macro
#define MARSHALLING_BIN_ENUM(EnumType)\
	namespace network2 { namespace marshalling {\
		inline cPacket& operator<<(cPacket& packet\
			, const EnumType::Enum& rhs) {\
			packet << (int)rhs;\
			return packet;\
		}\
		inline cPacket& operator>>(cPacket& packet\
			, OUT EnumType::Enum &rhs) {\
			int tmp;\
			packet >> tmp;\
			rhs = (EnumType::Enum)tmp;\
			return packet;\
		}\
	}}


// enum class
#define MARSHALLING_BIN_ENUM2(EnumType)\
	namespace network2 { namespace marshalling {\
		inline cPacket& operator<<(cPacket& packet\
			, const EnumType &rhs) {\
			packet << (int)rhs;\
			return packet;\
		}\
		inline cPacket& operator>>(cPacket& packet\
			, OUT EnumType &rhs) {\
			int tmp;\
			packet >> tmp;\
			rhs = (EnumType)tmp;\
			return packet;\
		}\
	}}


	// enum Marshalling Macro
#define MARSHALLING_ASCII_ENUM(EnumType)\
	namespace network2 { namespace marshalling_ascii {\
		inline cPacket& operator<<(cPacket& packet\
			, const EnumType::Enum& rhs) {\
			packet << (int)rhs;\
			return packet;\
		}\
		inline cPacket& operator>>(cPacket& packet\
			, OUT EnumType::Enum &rhs) {\
			int tmp;\
			packet >> tmp;\
			rhs = (EnumType::Enum)tmp;\
			return packet;\
		}\
	}}

// enum class
#define MARSHALLING_ASCII_ENUM2(EnumType)\
	namespace network2 { namespace marshalling_ascii {\
		inline cPacket& operator<<(cPacket& packet\
			, const EnumType &rhs) {\
			packet << (int)rhs;\
			return packet;\
		}\
		inline cPacket& operator>>(cPacket& packet\
			, OUT EnumType &rhs) {\
			int tmp;\
			packet >> tmp;\
			rhs = (EnumType)tmp;\
			return packet;\
		}\
	}}



	// sequence Marshalling Macro
#define MARSHALLING_BIN_APPEND_SEQ(packet, seq)\
		{\
			packet << (int)seq.size();\
			for (const auto &v : seq)\
				packet << v;\
		}

#define MARSHALLING_ASCII_APPEND_SEQ(packet, delimeter, seq)\
		{\
			int i = 0;\
			for (auto &v : seq)\
			{\
				packet << v;\
				if (i++ < ((int)seq.size() - 1))\
					packet.AppendDelimeter(delimeter);\
			}\
		}


#define MARSHALLING_BIN_GET_SEQ(packet, SequenceType, seq)\
		{\
			int size = 0;\
			packet >> size;\
			if (size < 0)\
				return packet;\
			if (size > 10000)\
				return packet;\
			typedef SequenceType::value_type type;\
			seq.reserve(size);\
			for (int i = 0; i < size; ++i)\
			{\
				type t;\
				packet >> t;\
				seq.push_back(t);\
			}\
		}


	// no reserve() function
#define MARSHALLING_BIN_GET_SEQ2(packet, SequenceType, seq)\
		{\
			int size = 0;\
			packet >> size;\
			if (size < 0)\
				return packet;\
			if (size > 10000)\
				return packet;\
			typedef SequenceType::value_type type;\
			for (int i = 0; i < size; ++i)\
			{\
				type t;\
				packet >> t;\
				seq.insert(t);\
			}\
		}


#define MARSHALLING_ASCII_GET_SEQ(packet, delimeter, SequenceType, seq)\
		{\
			typedef SequenceType::value_type type;\
			const int MAX_LOOP = 10000;\
			int cnt = 0;\
			while (cnt++ < MAX_LOOP)\
			{\
				type t;\
				packet >> t;\
				if (!packet.m_emptyData)\
					seq.push_back(t);\
				if (packet.m_lastDelim != delimeter)\
					break;\
			}\
			if (MAX_LOOP <= cnt)\
				assert(0);\
		}

}
