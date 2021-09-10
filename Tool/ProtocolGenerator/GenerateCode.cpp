
#include "pch.h"
#include "GenerateCode.h"
#include <direct.h>

using namespace network2;
using namespace std;

namespace compiler
{
	enum class eFormatType { Binary, Ascii, Json, };


	// Write Protocol Handler Code
	bool WriteFirstHandlerHeader(sProtocol *protocol, const string &marshallingName);
	bool WriteHandlerHeader(ofstream &fs, sProtocol *protocol);
	bool WriteFirstHandlerCpp(sProtocol *protocol, const string &pchFileName);
	bool WriteHandlerCpp(ofstream &fs, sProtocol *protocol);


	void WriteDeclPacketList(ofstream &fs, sPacket *packet
		, bool isVirtual, bool isImpl, bool isTarget, eFormatType format);
	void WriteFirstArg(ofstream &fs, sArg*p, bool isTarget, eFormatType format);
	void WriteArg(ofstream &fs, sArg *arg, bool comma);
	void WriteArgVar(ofstream &fs, sArg *arg, bool comma);


	// Write Protocol Data Code
	bool WriteFirstDataHeader(sProtocol *protocol, const string &marshallingName);
	bool WriteProtocolDataHeader(ofstream &fs, sProtocol *protocol);
	bool WritePacketData(ofstream &fs, sPacket *packet);
	void WritePacketDataArg(ofstream &fs, sArg *arg);


	// Write Dispatcher Code
	void WriteProtocolDispatchFunc(ofstream &fs, sProtocol *protocol, eFormatType format);
	void WriteDispatchSwitchCase(ofstream &fs, sPacket *packet, eFormatType format);
	void WriteDispatchImpleArg(ofstream &fs, sArg*p, const string &tab, bool isJson, bool isBinary);
	void WriteLastDispatchSwitchCase(ofstream &fs, sPacket *packet, const string &tab);


	// Write Protocol Code
	bool WriteFirstProtocolClassHeader(sProtocol *protocol, const string &marshallingName);
	bool WriteProtocolClassHeader(ofstream &fs, sProtocol *protocol);
	bool WriteFirstProtocolCpp(sProtocol *protocol, const string &pchFileName, eFormatType format);
	bool WriteProtocolCpp(ofstream &fs, sProtocol *protocol, eFormatType format);

	void WriteImplPacketList(ofstream &fs, sPacket *packet, eFormatType format);
	void WriteFirstImplePacket(ofstream &fs, sPacket *packet, sArg*p, eFormatType format);
	void WriteImpleArg(ofstream &fs, sArg*p, const string &tab, eFormatType format, bool isJsonPut=false);
	void WriteLastImplePacket(ofstream &fs, const string &tab);

	string GetProtocolName(const string &fileName);
	string GetProtocolClassName(const string &protocolName );
	string GetProtocolHandlerClassName(const string &protocolName );
	string GetProtocolDispatcherClassName(const string &protocolName );


	string g_className; // protocol name + ('_Protocol' | '_ProtocolHandler' | '_Dispatcher')
	string g_protocolId; // protocol name + '_ID'
	string g_protocolName;	// *.prt filename except extends name
	string g_origianlFileName; // protocol name with directory path
	string g_handlerClassName; // protocol handler class name
	string g_packetHeaderClassName; // protocol packet header class name
									// cPacketHeader, cPacketHeaderAscii, cPacketHeaderJson
	string g_srcFolderName = "Src";
}

using namespace compiler;


//------------------------------------------------------------------------
// generate Protocol, Handler, Dispatcher, Packet Data class
//------------------------------------------------------------------------
bool compiler::WriteProtocolCode(const string &protocolFileName, sProtocol *protocol
	, const string &pchFileName
)
{
	const string fileName = common::GetFileNameExceptExt(protocolFileName);
	const string path = common::GetFilePathExceptFileName(protocolFileName);
	const string folder = (path.empty())? g_srcFolderName : path + "\\" + g_srcFolderName;
	_mkdir(folder.c_str());
	g_origianlFileName = folder + "\\" + fileName;

	eFormatType format = eFormatType::Binary;
	string marshallingName = "marshalling"; // default binary marshalling
	if (protocol->format == "ascii")
	{
		format = eFormatType::Ascii;
		marshallingName = "marshalling_ascii";
	}
	else if (protocol->format == "json")
	{
		format = eFormatType::Json;
		marshallingName = "marshalling_json";
	}

	g_protocolName = GetProtocolName(protocolFileName);

	WriteFirstProtocolClassHeader(protocol, marshallingName);
	WriteFirstProtocolCpp(protocol, pchFileName, format);

	WriteFirstHandlerHeader(protocol, marshallingName);
	WriteFirstHandlerCpp(protocol, pchFileName);

	WriteFirstDataHeader(protocol, marshallingName);
	return true;
}


//------------------------------------------------------------------------
// return filename except extends name
//------------------------------------------------------------------------
string compiler::GetProtocolName(const string &fileName)
{
	return common::GetFileNameExceptExt(fileName);
}


//------------------------------------------------------------------------
// return protocol class name, protocol name + _Protocol
//------------------------------------------------------------------------
string compiler::GetProtocolClassName(const string &protocolName )
{
	return protocolName + "_Protocol";
}


//------------------------------------------------------------------------
// return protocol handler class name, protocol name + _ProtocolHandler
//------------------------------------------------------------------------
string compiler::GetProtocolHandlerClassName(const string &protocolName )
{
	return protocolName + "_ProtocolHandler";
}


//------------------------------------------------------------------------
// return protocol dispatcher class name, protocol name + _Dispatcher
//------------------------------------------------------------------------
string compiler::GetProtocolDispatcherClassName(const string &protocolName )
{
	return protocolName + "_Dispatcher";
}


//------------------------------------------------------------------------
// Protocol Header 파일을 생성하는 처음부분의 코드를 생성한다.
//------------------------------------------------------------------------
bool compiler::WriteFirstProtocolClassHeader(sProtocol *protocol, const string &marshallingName)
{
	const string fileName = g_origianlFileName + "_Protocol.h";

	std::ofstream fs;
	fs.open(fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteProtocolClassHeader(fs, protocol);

	fs << "}\n";
	return true;
}


//------------------------------------------------------------------------
// generate Protocol.h code
//------------------------------------------------------------------------
bool compiler::WriteProtocolClassHeader(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolClassName(protocol->name);
	g_protocolId = g_className + "_ID";

	eFormatType format = eFormatType::Binary;
	string packetFormat = "ePacketFormat::BINARY";
	g_packetHeaderClassName = "cPacketHeader";
	if (protocol->format == "ascii")
	{
		format = eFormatType::Ascii;
		packetFormat = "ePacketFormat::ASCII";
		g_packetHeaderClassName = "cPacketHeaderAscii";
	}
	else if (protocol->format == "json")
	{
		format = eFormatType::Json;
		packetFormat = "ePacketFormat::JSON";
		g_packetHeaderClassName = "cPacketHeaderJson";
	}

	fs << "static const int " << g_protocolId << " = " << protocol->number << ";\n";
	//fs << "static " << packetHeader << " g_" << protocol->name << "_PacketHeader;\n";
	fs << endl;
	fs << "class " << g_className <<  " : public network2::iProtocol\n";
	fs << "{" << endl;
	fs << "public:" << endl;
	fs << "\t" << g_className << "() : iProtocol(" << g_protocolId 
		<< ", " << packetFormat << ") {}\n";
	WriteDeclPacketList( fs, protocol->packet, false, false, true, format);
	fs << "\tstatic " << g_packetHeaderClassName << " s_" << "packetHeader;\n";
	fs << "};\n";

	return WriteProtocolClassHeader(fs, protocol->next);
}


/**
 @brief WriteFirstDataHeader
 */
bool compiler::WriteFirstDataHeader(sProtocol *protocol, const string &marshallingName)
{
	const string fileName = g_origianlFileName + "_ProtocolData.h";

	ofstream fs;
	fs.open( fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteProtocolDataHeader(fs, protocol);

	fs << "}\n";
	return true;
}


/**
 @brief WriteDataHeader
 */
bool compiler::WriteProtocolDataHeader(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolDispatcherClassName(protocol->name);
	g_protocolId = g_className + "_ID";

	fs << endl;
	fs << endl;

	WritePacketData(fs, protocol->packet );

	fs << endl;
	fs << endl;

	return WriteProtocolDataHeader(fs, protocol->next);
}


/**
 @brief 
 */
bool compiler::WritePacketData(ofstream &fs, sPacket *packet)
{
	if (!packet) return true;

	fs << "\tstruct " << packet->name << "_Packet {" << endl;
	fs << "\t\tcProtocolDispatcher *pdispatcher;\n";
	fs << "\t\tnetid senderId;\n";
	WritePacketDataArg(fs, packet->argList );
	fs << "\t};";
	fs << endl;
	fs << endl;

	return WritePacketData(fs, packet->next);
}


/**
 @brief 
 */
void compiler::WritePacketDataArg(ofstream &fs, sArg *arg)
{
	if (!arg) return;
	fs << "\t\t" << arg->var->type << " " << arg->var->var << ";" << endl;
	WritePacketDataArg(fs, arg->next);
}


//------------------------------------------------------------------------
// ProtocolHandler 헤더파일에서 처음 들어갈 주석 코드 추가
//------------------------------------------------------------------------
bool compiler::WriteFirstHandlerHeader(sProtocol *protocol, const string &marshallingName)
{
	const string fileName = g_origianlFileName + "_ProtocolHandler.h";

	ofstream fs;
	fs.open(fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "#include \"" << g_protocolName << "_ProtocolData.h\"" << endl;
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteHandlerHeader(fs, protocol);

	fs << "}\n";
	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler 헤더 클래스 소스파일을 생성한다.
//------------------------------------------------------------------------
bool compiler::WriteHandlerHeader(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolDispatcherClassName(protocol->name);
	g_protocolId = g_className + "_ID";

	eFormatType format = eFormatType::Binary;
	string packetFormat = "ePacketFormat::BINARY";
	g_packetHeaderClassName = "cPacketHeader";
	if (protocol->format == "ascii")
	{
		format = eFormatType::Ascii;
		packetFormat = "ePacketFormat::ASCII";
		g_packetHeaderClassName = "cPacketHeaderAscii";
	}
	else if (protocol->format == "json")
	{
		format = eFormatType::Json;
		packetFormat = "ePacketFormat::JSON";
		g_packetHeaderClassName = "cPacketHeaderJson";
	}

	fs << "static const int " << g_protocolId << " = " << protocol->number << ";\n";
	fs << endl;

	// Protocol Dispatcher
	fs << "// Protocol Dispatcher\n";
	fs << "class " << g_className << ": public network2::cProtocolDispatcher\n";
	fs << "{\n";
	fs << "public:\n";
	fs << "\t" << g_className << "();\n";
	fs << "protected:\n";
	fs << "\tvirtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;\n";
	fs << "\tstatic " << g_packetHeaderClassName << " s_" << "packetHeader;\n";
	fs << "};\n";
	fs << "static " << g_className << " g_" << g_protocolName << "_" << g_className << ";\n";// 전역변수 선언
	fs << endl;
	fs << endl;

	// CProtocolHandler class
	g_className = GetProtocolHandlerClassName(protocol->name);
	string dispatcherClassName = GetProtocolDispatcherClassName(protocol->name);

	fs << "// ProtocolHandler\n";
	fs << "class " << g_className << " : virtual public network2::iProtocolHandler\n";
	fs << "{\n";
	fs << "public:\n";
	fs << "\tfriend class " << dispatcherClassName << ";\n";
	fs << "\t" << g_className << "() { m_format = " << packetFormat << "; }\n";
	WriteDeclPacketList( fs, protocol->packet, true, true, false, format);
	fs << "};\n";
	fs << endl;
	fs << endl;

	return WriteHandlerHeader(fs, protocol->next);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool compiler::WriteFirstHandlerCpp(sProtocol *protocol, const string &pchFileName)
{
	const string fileName = g_origianlFileName + "_ProtocolHandler.cpp";
	const string headerFileName = g_protocolName + "_ProtocolHandler.h";

	ofstream fs(fileName.c_str());
	if (!fs.is_open())
		return false;

	if (!pchFileName.empty())
		fs << "#include \"" << pchFileName << "\"\n";
	fs << "#include \"" << headerFileName << "\"\n";
	fs << endl;
	fs << "using namespace " << g_protocolName << ";\n";
	fs << endl;

	WriteHandlerCpp(fs, protocol);
	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler Cpp 파일을 생성한다.
//------------------------------------------------------------------------
bool compiler::WriteHandlerCpp(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolDispatcherClassName(protocol->name);
	g_protocolId = g_className + "_ID";

	// Dispatcher 생성자 코드 생성
	eFormatType format = eFormatType::Binary;
	string packetFormat = "ePacketFormat::BINARY";
	if (protocol->format == "ascii")
	{
		format = eFormatType::Ascii;
		packetFormat = "ePacketFormat::ASCII";
	} else if (protocol->format == "json")
	{
		format = eFormatType::Json;
		packetFormat = "ePacketFormat::JSON";
	}

	fs << "\n";
	fs << g_packetHeaderClassName << " " << g_protocolName << "::" << g_className;
	fs << "::s_packetHeader;\n";

	fs << g_protocolName << "::" << g_className << "::" << g_className << "()\n";
	fs << "\t: cProtocolDispatcher(" << g_protocolName << "::" << g_protocolId 
		<< ", " << packetFormat << ")\n";
	fs << "{\n";
	fs << "\tcProtocolDispatcher::GetDispatcherMap()->insert({" << g_protocolId << ", this});\n";
	fs << "\tcProtocolDispatcher::GetPacketHeaderMap()->insert({" << g_protocolId << ", &s_packetHeader});\n";
	fs << "}\n";
	fs << endl;
	//

	// Dispatcher 클래스의 Dispatch() 함수 코드 생성
	WriteProtocolDispatchFunc(fs, protocol, format);
	fs << endl;
	fs << endl;
	fs << endl;

	return WriteHandlerCpp(fs, protocol->next);
}


//------------------------------------------------------------------------
// in Protocol.h
// generate packet function list code
//------------------------------------------------------------------------
void compiler::WriteDeclPacketList(ofstream &fs, sPacket *packet
	, bool isVirtual, bool isImpl, bool isTarget, eFormatType format)
{
	if (!packet) return;
	
	if (!isImpl) // protocol header file?
	{
		fs << "\t";
		if (isVirtual)
			fs << "virtual ";

		fs << "void ";

		fs << packet->name << "(";
		WriteFirstArg(fs, packet->argList, isTarget, format);
		fs << ")";
		if (isImpl)
			fs << "{ return true; }"; // Handler header file
		else
			fs << ";"; // protocol header file
		fs << endl;
	}

	// Second Interface
	if (isVirtual && isImpl) // handler header file?
	{
		fs << "\t";
		fs << "virtual ";
		fs << "bool "; // Handler header file
		fs << packet->name << "(";
		fs << g_protocolName << "::";
		fs << packet->name << "_Packet &packet";
		fs << ") { return true; }"; // Handler header file
		fs << endl;
	}

	WriteDeclPacketList(fs, packet->next, isVirtual, isImpl, isTarget, format);
}


//------------------------------------------------------------------------
// in Protocol.cpp
// generate packet send function code
//------------------------------------------------------------------------
void compiler::WriteImplPacketList(ofstream &fs, sPacket *packet, eFormatType format)
{
	if (!packet) return;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// Protocol: " << packet->name << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "void " << g_protocolName << "::" << g_className << "::" << packet->name << "(";
	WriteFirstArg(fs, packet->argList, true, format);
	fs << ")\n";
	fs << "{\n";
	WriteFirstImplePacket(fs, packet, packet->argList, format);
	fs << "}\n";
	fs << "\n";
	WriteImplPacketList(fs, packet->next, format);
}


//------------------------------------------------------------------------
// generate make packet argument code
//------------------------------------------------------------------------
void compiler::WriteFirstArg(ofstream &fs, sArg*p, bool isTarget, eFormatType format)
{
	if (isTarget)
	{
		fs <<"netid targetId";

		if (format == eFormatType::Json) {
			fs << ", bool isBinary";
		}
	}
	else
	{
		fs << "cProtocolDispatcher &dispatcher, ";
		fs << "netid senderId";
	}

	WriteArg(fs, p, true);
}
void compiler::WriteArg(ofstream &fs, sArg *arg, bool comma)
{
	if (!arg) return;
	if (comma)
		fs << ", ";
	fs << "const " << arg->var->type << " &" << arg->var->var;
	WriteArg(fs, arg->next, true);
}

//------------------------------------------------------------------------
// 프로토콜 인자값의 변수 이름만 출력한다. (함수 호출시 사용)
//------------------------------------------------------------------------
void compiler::WriteArgVar(ofstream &fs, sArg *arg, bool comma)
{
	if (!arg) return;
	if (comma)
		fs << ", ";
	fs << arg->var->var;
	WriteArgVar(fs, arg->next, true);
}


//------------------------------------------------------------------------
// in Protocol Class
// generate make packet and send code
//------------------------------------------------------------------------
void compiler::WriteFirstImplePacket(ofstream &fs, sPacket *packet
	, sArg *p, eFormatType format)
{
	//fs << "\tcPacket packet(m_node->GetPacketHeader());\n";
	fs << "\tcPacket packet(&s_packetHeader);\n";

	fs << "\tpacket.SetProtocolId( GetId() );\n";
	fs << "\tpacket.SetPacketId( " << packet->packetId << " );\n";

	if (format == eFormatType::Json)
	{
		fs << "\tpacket.SetPacketOption(0x01, (uint)isBinary);\n";
		fs << "\tif (isBinary)\n"; // begin if (isBinary)
		fs << "\t{\n";

		// marshalling binary
		fs << "\t\t// marshaling binary\n";
		fs << "\t\tpacket.Alignment4(); // set 4byte alignment\n";
		WriteImpleArg(fs, p, "\t\t", format, false);
		fs << "\t\tpacket.EndPack();\n";
		WriteLastImplePacket(fs, "\t\t");

		fs << "\t}\n";
		fs << "\telse\n"; // else
		fs << "\t{\n";

		// marshalling json
		fs << "\t\t// marshaling json\n";
		fs << "\t\tusing boost::property_tree::ptree;\n";
		fs << "\t\tptree props;\n";
		fs << "\t\ttry {\n";
		WriteImpleArg(fs, p, "\t\t\t", format, true);
		fs << "\t\t\tstringstream ss;\n";
		fs << "\t\t\tboost::property_tree::write_json(ss, props);\n";
		fs << "\t\t\tpacket << ss.str();\n";
		fs << "\t\t\tpacket.EndPack();\n";
		WriteLastImplePacket(fs, "\t\t\t");
		fs << "\t\t} catch (...) {\n";
		fs << "\t\t\tdbg::Logp(\"json packet maker error\\n\");\n";
		fs << "\t\t}\n";
		fs << "\t}\n"; // end else if (isBinary)
	}
	else
	{
		// binary or ascii format packet
		WriteImpleArg(fs, p, "\t\t", format);
		fs << "\tpacket.EndPack();\n";
		WriteLastImplePacket(fs, "\t");
	}
}


//------------------------------------------------------------------------
// in Protocol class
// generate make packet code
//------------------------------------------------------------------------
void compiler::WriteImpleArg(ofstream &fs, sArg*p, const string &tab
	, eFormatType format, bool isJsonPut //= false
)
{
	if (!p) return;

	switch (format)
	{
	case eFormatType::Json:
		if (isJsonPut)
			fs << tab << "put(props, \"" << p->var->var << "\", " << p->var->var << ");\n";
		else
			fs << tab << "marshalling::operator<<(packet, " << p->var->var << ");\n";
		break;
	case eFormatType::Ascii:
		fs << "\tpacket << " << p->var->var << ";\n";
		fs << "\tAddDelimeter(packet);\n";
		break;
	case eFormatType::Binary:
		fs << "\tpacket << " << p->var->var << ";\n";
		break;
	default: assert(0); break;
	}

	WriteImpleArg(fs, p->next, tab, format, isJsonPut);
}


//------------------------------------------------------------------------
// in Protocol class
// generate send packet code 
//------------------------------------------------------------------------
void compiler::WriteLastImplePacket(ofstream &fs, const string &tab)
{
	fs << tab << "GetNode()->Send(targetId, packet);\n";
}


//------------------------------------------------------------------------
// generate protocol.cpp code
//------------------------------------------------------------------------
bool compiler::WriteFirstProtocolCpp(sProtocol *protocol, const string &pchFileName, eFormatType format)
{
	const string fileName = g_origianlFileName + "_Protocol.cpp";
	const string headerFileName = g_protocolName + "_Protocol.h";

	ofstream fs(fileName.c_str());
	if (!fs.is_open()) return false;

	if (!pchFileName.empty())
		fs << "#include \"" << pchFileName << "\"\n";
	fs << "#include \"" << headerFileName << "\"\n";

	fs << "using namespace " << g_protocolName << ";\n";
	fs << endl;

	WriteProtocolCpp(fs, protocol, format);

	return true;
}


//------------------------------------------------------------------------
// generate protocol.cpp code
//------------------------------------------------------------------------
bool compiler::WriteProtocolCpp(ofstream &fs, sProtocol *protocol, eFormatType format)
{
	if (!protocol) return true;
	
	g_className = GetProtocolClassName(protocol->name);

	// protocol packet header declare
	fs << g_packetHeaderClassName << " " << g_protocolName << "::" << g_className;
	fs << "::s_packetHeader;\n";

	WriteImplPacketList( fs, protocol->packet, format);
	fs << endl;
	fs << endl;

	return WriteProtocolCpp(fs, protocol->next, format);
}


//------------------------------------------------------------------------
// generate Dispatcher::Dispatch() code
//------------------------------------------------------------------------
void compiler::WriteProtocolDispatchFunc(ofstream &fs, sProtocol *protocol, eFormatType format)
{
	g_handlerClassName = GetProtocolHandlerClassName(protocol->name);

	fs << "//------------------------------------------------------------------------\n";
	fs << "// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.\n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "bool " << g_protocolName << "::" << g_className << "::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)\n", 
	fs << "{\n";
		fs << "\tconst int protocolId = packet.GetProtocolId();\n";
		fs << "\tconst int packetId = packet.GetPacketId();\n";

	if (protocol->packet)
	{
		fs << "\tswitch (packetId)\n";
		fs << "\t{\n";

		WriteDispatchSwitchCase(fs, protocol->packet, format);

		fs << "\tdefault:\n";
			fs << "\t\tassert(0);\n";
			fs << "\t\tbreak;\n";
		fs << "\t}\n";
	}
	fs << "\treturn true;\n";
	fs << "}\n";	
}


//------------------------------------------------------------------------
// generate Dispatcher switch case code
//------------------------------------------------------------------------
void compiler::WriteDispatchSwitchCase(ofstream &fs, sPacket *packet, eFormatType format)
{
	if (!packet) return;

	fs << "\tcase " << packet->packetId << ": // " << packet->name << "\n";
	fs << "\t\t{\n";

	// call HandlerMatching<T> function 
	fs << "\t\t\tProtocolHandlers prtHandler;\n";
	fs << "\t\t\tif (!HandlerMatching<" << g_handlerClassName << ">(handlers, prtHandler))\n";
	fs << "\t\t\t\treturn false;\n";
	fs << endl;

	// set current packet
	fs << "\t\t\tSetCurrentDispatchPacket( &packet );\n";
	fs << endl;

	// json packet parsing object
	string tab = "\t\t\t";
	if (format == eFormatType::Json)
	{
		// check option bit, whether it is binary or json string
		fs << tab << "const bool isBinary = packet.GetPacketOption(0x01) > 0;\n";

		fs << tab << "if (isBinary)\n";
		fs << tab << "{\n";
		tab += "\t";

		// binary packet parsing
		fs << tab << "// binary parsing\n";

		fs << tab << packet->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		fs << tab << "packet.Alignment4(); // set 4byte alignment\n";
		WriteDispatchImpleArg(fs, packet->argList, tab, true, true);
		WriteLastDispatchSwitchCase(fs, packet, tab);

		tab = "\t\t\t";
		fs << tab << "}\n";
		fs << tab << "else\n"; // else
		fs << tab << "{\n";
		tab += "\t";

		// json packet parsing
		fs << tab << "// json format packet parsing using property_tree\n";
		fs << tab << "using boost::property_tree::ptree;\n";
		fs << tab << "ptree root;\n\n";
		fs << tab << "try {\n";
		fs << tab << "\tstring str;\n";
		fs << tab << "\tpacket >> str;\n";
		fs << tab << "\tstringstream ss(str);\n";
		fs << tab << "\t\n";
		fs << tab << "\tboost::property_tree::read_json(ss, root);\n";
		fs << tab << "\tptree &props = root.get_child(\"\");\n\n";
		tab += "\t";

		fs << tab << packet->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		WriteDispatchImpleArg(fs, packet->argList, tab, true, false);
		WriteLastDispatchSwitchCase(fs, packet, tab);

		tab = "\t\t\t\t";
		fs << tab << "} catch (...) {\n";
		fs << tab << "\tdbg::Logp(\"json packet parsing error packetid = %lu\\n\", packetId);\n";
		fs << tab << "}\n";

		fs << "\t\t\t}\n";
	}
	else
	{
		fs << tab << packet->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		WriteDispatchImpleArg(fs, packet->argList, tab, false, true);
		WriteLastDispatchSwitchCase(fs, packet, tab);
	}

	fs << "\t\t}\n";
	fs << "\t\tbreak;\n";
	fs << "\n";

	WriteDispatchSwitchCase(fs, packet->next, format);
}


//------------------------------------------------------------------------
// in dispatcher class
// read packet data and save protocol data structure
//------------------------------------------------------------------------
void compiler::WriteDispatchImpleArg(ofstream &fs, sArg*p, const string &tab
	, bool isJson, bool isBinary)
{
	if (!p) return;
	
	if (isJson)
	{
		if (isBinary)
		{
			fs << tab << "marshalling::operator>>(packet, "
				<< "data." << p->var->var << ");\n";
		}
		else
		{
			fs << tab << "get(props, \"" << p->var->var <<
				"\", data." << p->var->var << ");\n";
		}
	}
	else 
	{
		fs << tab << "packet >> " << "data." << p->var->var << ";\n";
	}	
	WriteDispatchImpleArg(fs, p->next, tab, isJson, isBinary);
}

//------------------------------------------------------------------------
// in dispatcher class
// generate call handler function code
// ex) SEND_HANDLER(c2s_ProtocolHandler, prtHandler, SpawnRobot(data));
//------------------------------------------------------------------------
void compiler::WriteLastDispatchSwitchCase(ofstream &fs, sPacket *packet, const string &tab)
{
	fs << tab << "SEND_HANDLER(" << g_handlerClassName << ", prtHandler, " 
		<< packet->name << "(data));\n";
}
