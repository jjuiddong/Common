
#include "stdafx.h"
#include "ProtocolDefine.h"


using namespace network2;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void network2::PrintToken( eTokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING: printf( "string = %s\n", szTokenString ); break;

	case ASSIGN: printf( "=" ); break;
	case TIMES:	printf( "*" ); break;
	case LPAREN: printf( "(" ); break;
	case RPAREN: printf( ")" ); break;
	case LBRACE: printf( "{" ); break;
	case RBRACE: printf( "}" ); break;
	case COMMA:	printf( "," ); break;
	case NUM: printf( "number" ); break;
	case OR: printf( "||" ); break;
	case AND: printf( "&&" ); break;
	default: printf( "UnKnown token %d, '%s'\n", token, szTokenString ); break;
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
	case ePacketFormat::JSON: v =(T)0; break;
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
	case ePacketFormat::JSON: v = ""; break;
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
	else if (typeStr == "int64")
		Packet2Variant<__int64>(format, packet, v);
	else if (typeStr == "uint64")
		Packet2Variant<unsigned __int64>(format, packet, v);
	else if (typeStr == "unsigned __int64")
		Packet2Variant<unsigned __int64>(format, packet, v);
	else if (typeStr == "__int64")
		Packet2Variant<__int64>(format, packet, v);
}


////------------------------------------------------------------------------
//// packetID를 리턴한다. 여기서 Packet이란 sProtocol protocol을 의미하고, 
//// sProtocol 의 자식으로 순서대로 번호가 매겨진 값이 Packet ID이다.
////------------------------------------------------------------------------
//int	network2::GetPacketID(sRmi *rmi, sPacket *packet)
//{
//	if (!rmi) return 0;
//	if (!packet) return rmi->number;
//
//	int id = rmi->number + 1;
//	sPacket *p = rmi->packet;
//	while (p)
//	{
//		if (p == packet)
//			break;
//		++id;
//		p = p->next;
//	}
//	return id;
//}

template<class T>
void Vector2String(cPacket &packet, const bool isBinaryPacket
	, sArg *arg, INOUT stringstream &ss)
{
	vector<T> vecs;
	if (isBinaryPacket)
	{
		using namespace marshalling;
		packet >> vecs;
	}
	else
	{
		using namespace marshalling_ascii;
		packet >> vecs;
	}

	ss << arg->var->var + " = {";
	for (auto &v : vecs)
		ss << v << ",";
	ss << "}";
}


//------------------------------------------------------------------------
// convert packet data to string
// no optimize, use debugging mode
//------------------------------------------------------------------------
string network2::Packet2String(const cPacket &packet, sPacket *protocol)
{
	if (!protocol)
	{
		return format( " protocol id = %d, packet id = %u"
			, packet.GetProtocolId(), packet.GetPacketId() );
	}

	std::stringstream ss;
	cPacket tempPacket;
	tempPacket.ShallowCopy(packet);

	tempPacket.InitRead();

	const int protocolID = tempPacket.GetProtocolId();
	const uint packetID = tempPacket.GetPacketId();
	ePacketFormat format = GetPacketFormat(tempPacket);
	if (ePacketFormat::JSON == format)
	{
		const uint option = tempPacket.GetPacketOption(0x01);
		if (option)
		{
			tempPacket.Alignment4();
			format = ePacketFormat::BINARY;
		}
	}
	const bool isBinaryPacket = ePacketFormat::BINARY == format;

	ss << protocol->name << " sender = " << tempPacket.GetSenderId() << " ";

	sArg *arg = protocol->argList;
	while (arg)
	{
		// todo: need more generalize code
		// not work vector<float>, vector<char> type
		const string &type = arg->var->type;
		if (type == "vector<int>")
			Vector2String<int>(tempPacket, isBinaryPacket, arg, ss);
		else if (type == "vector<ushort>")
			Vector2String<ushort>(tempPacket, isBinaryPacket, arg, ss);
		else if (type == "vector<uint>")
			Vector2String<uint>(tempPacket, isBinaryPacket, arg, ss);
		else if (type == "vector<string>")
			Vector2String<string>(tempPacket, isBinaryPacket, arg, ss);
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


// release statement structure
void network2::ReleaseStmt(sStmt *p)
{
	if (!p) return;
	ReleaseProtocol(p->protocol);
	ReleaseType(p->type);
	ReleaseStmt(p->next);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseProtocol(sProtocol *p)
{
	if (!p) return;
	ReleasePacket(p->packet);
	ReleaseProtocol(p->next);
	delete p;
}


/**
 @brief ReleaseRmiOnly
 */
void network2::ReleaseProtocolOnly(sProtocol *p)
{
	if (!p) return;
	ReleaseProtocolOnly(p->next);
	delete p;
}


/**
 @brief ReleaseRmiOnly
 */
void network2::ReleaseProtocolOnly(sStmt *p)
{
	if (!p) return;
	ReleaseProtocolOnly(p->protocol);
	ReleaseProtocolOnly(p->next);
	delete p;
}


// release type structure
void network2::ReleaseType(sType *p)
{
	if (!p) return;
	ReleaseArg(p->vars);
	ReleaseType(p->next);
	delete p;
}


/**
 @brief 
 */
void network2::ReleasePacket(sPacket *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	ReleasePacket(p->next);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseCurrentPacket(sPacket *p)
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