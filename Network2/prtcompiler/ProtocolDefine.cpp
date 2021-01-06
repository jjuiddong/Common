
#include "stdafx.h"
#include "ProtocolDefine.h"


using namespace network2;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void network2::PrintToken( Tokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING:	printf( "string = %s\n", szTokenString ); break;

	case ASSIGN:	printf( "=" ); break;
	case TIMES:		printf( "*" ); break;
	case LPAREN:	printf( "(" ); break;
	case RPAREN:	printf( ")" ); break;
	case LBRACE:	printf( "{" ); break;
	case RBRACE:	printf( "}" ); break;
	case COMMA:	printf( "," ); break;
	case NUM:		printf( "number" ); break;
	case OR:		printf( "||" ); break;
	case AND:		printf( "&&" ); break;

	default: 		printf( "UnKnown token %d, %s\n", token, szTokenString ); break;
	}
}


// packet element parsing
template <class T>
void Packet2Variant(const ePacketFormat format, cPacket &packet, OUT _variant_t &var)
{
	T v;
	switch (format) {
	case ePacketFormat::BINARY: marshalling::operator>>(packet, v); break;
	case ePacketFormat::ASCII: marshalling_ascii::operator>>(packet, v); break;
	case ePacketFormat::JSON: break;
	}
	var = v;
}
// string specialization
template<>
void Packet2Variant<std::string>(const ePacketFormat format, cPacket &packet, OUT _variant_t &var)
{
	string v;
	switch (format) {
	case ePacketFormat::BINARY: marshalling::operator>>(packet, v); break;
	case ePacketFormat::ASCII: marshalling_ascii::operator>>(packet, v); break;
	case ePacketFormat::JSON: break;
	}
	var = v.c_str();
}

//------------------------------------------------------------------------
// packet parsing with typeStr, and then return variant value
// typeStr: sArg->var->type
//------------------------------------------------------------------------
void network2::GetPacketElement(const ePacketFormat format
	, const string &typeStr, cPacket &packet, OUT _variant_t &v)
{
	if (typeStr == "std::string")
		Packet2Variant<string>(format, packet, v);
	if (typeStr == "string")
		Packet2Variant<string>(format, packet, v);
	else if (typeStr == "float")
		Packet2Variant<float>(format, packet, v);
	else if (typeStr == "double")
		Packet2Variant<double>(format, packet, v);
	else if (typeStr == "int")
		Packet2Variant<int>(format, packet, v);
	else if (typeStr == "uint")
		Packet2Variant<unsigned int>(format, packet, v);
	else if (typeStr == "unsigned int")
		Packet2Variant<unsigned int>(format, packet, v);
	else if (typeStr == "char")
		Packet2Variant<char>(format, packet, v);
	else if (typeStr == "unsigned char")
		Packet2Variant<unsigned char>(format, packet, v);
	else if (typeStr == "BYTE")
		Packet2Variant<BYTE>(format, packet, v);
	else if (typeStr == "short")
		Packet2Variant<short>(format, packet, v);
	else if (typeStr == "long")
		Packet2Variant<long>(format, packet, v);
	else if (typeStr == "bool")
		Packet2Variant<bool>(format, packet, v);
	else if (typeStr == "BOOL")
		Packet2Variant<bool>(format, packet, v);
}


//------------------------------------------------------------------------
// packetID를 리턴한다. 여기서 Packet이란 sProtocol protocol을 의미하고, 
// sRmi 의 자식으로 순서대로 번호가 매겨진 값이 Packet ID이다.
//------------------------------------------------------------------------
int	network2::GetPacketID(sRmi *rmi, sProtocol *packet)
{
	if (!rmi) return 0;
	if (!packet) return rmi->number;

	int id = rmi->number + 1;
	sProtocol *p = rmi->protocol;
	while (p)
	{
		if (p == packet)
			break;
		++id;
		p = p->next;
	}
	return id;
}


//------------------------------------------------------------------------
// 패킷내용을 스트링으로 변환 한다.
// 동적 메모리 할당이 많기 때문에, 디버깅 시에만 쓸것
//------------------------------------------------------------------------
string network2::Packet2String(const cPacket &packet, sProtocol *protocol)
{
	if (!protocol)
	{
		return format( " protocol id = %d, packet id = %d"
			, packet.GetProtocolId(), packet.GetPacketId() );
	}

	std::stringstream ss;
	cPacket tempPacket = packet;
	tempPacket.InitRead();

	const int protocolID = tempPacket.GetProtocolId();
	const int packetID = tempPacket.GetPacketId();
	const bool isBinaryPacket = dynamic_cast<cPacketHeader*>(tempPacket.m_packetHeader)? true : false;
	const ePacketFormat format = isBinaryPacket ?
		ePacketFormat::BINARY : ePacketFormat::ASCII;

	ss << protocol->name << " sender = " << tempPacket.GetSenderId() << " ";

	sArg *arg = protocol->argList;
	while (arg)
	{
		// todo: 좀더 일반적인 처리가 필요하다.
		// vector<float>, vector<char> 는 동작하지 않는다.
		if (arg->var->type == "vector<int>")
		{
			vector<int> vecs;
			if (isBinaryPacket)
			{
				using namespace marshalling;
				tempPacket >> vecs;
			}
			else
			{
				using namespace marshalling_ascii;
				tempPacket >> vecs;
			}

			ss << arg->var->var + " = ";
			for (auto &v : vecs)
				ss << v << ";";
		}
		//if (arg->var->type == "vector<ushort>")
		//{
		//	vector<ushort> vecs;
		//	if (isBinaryPacket)
		//	{
		//		using namespace marshalling;
		//		tempPacket >> vecs;
		//	}
		//	else
		//	{
		//		using namespace marshalling_ascii;
		//		tempPacket >> vecs;
		//	}
		//	ss << arg->var->var + " = ";
		//	for (auto &v : vecs)
		//		ss << v << ";";
		//}
		else
		{
			_variant_t var;
			GetPacketElement(format, arg->var->type, tempPacket, var);
			ss << arg->var->var + " = ";
			ss << common::variant2str(var);
		}

		//if ( arg->var->var == "errorCode")
		//	ss << "(" << ErrorCodeString((error::ERROR_CODE)(int)var) << ")";

		arg = arg->next;
		if (arg)
			ss << ", ";
	}

	return ss.str();
}


/**
 @brief 
 */
void network2::ReleaseRmi(sRmi *p)
{
	if (!p) return;
	ReleaseProtocol(p->protocol);
	ReleaseRmi(p->next);
	delete p;
}


/**
 @brief ReleaseRmiOnly
 */
void network2::ReleaseRmiOnly(sRmi *p)
{
	if (!p) return;
	ReleaseRmiOnly(p->next);
	delete p;
}

/**
 @brief 
 */
void network2::ReleaseProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	ReleaseProtocol(p->next);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseCurrentProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseArg(sArg *p)
{
	if (!p) return;
	if (p->var) delete p->var;
	ReleaseArg(p->next);
	delete p;
}