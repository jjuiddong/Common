
#include "pch.h"
#include "GenerateCode2.h"
#include <direct.h>

using namespace network2;
using namespace std;

namespace compiler2
{
	// Write Protocol Data Code
	bool WriteProtocolData(ofstream &fs, sProtocol *protocol);
	bool WriteCustomStruct(ofstream &fs, sType *type);
	bool WritePacketStruct(ofstream &fs, sPacket *packet);
	void WritePacketField(ofstream &fs, sArg *arg, const bool isNewLine = true
		, const bool isDecl =false);

	// Write Protocol Handler Code
	bool WriteDispatcher(ofstream &fs, sProtocol *protocol);
	bool WriteHandler(ofstream &fs, sProtocol *protocol);
	void WriteDeclPacketList(ofstream &fs, sPacket *packet);
	void WriteDeclPacketFirstArg(ofstream &fs, sArg*p);

	// Write Dispatcher Code
	void WriteProtocolDispatchFunc(ofstream &fs, sProtocol *protocol);
	void WriteProtocolDispatchAsyncFunc(ofstream &fs, sProtocol *protocol);
	void WriteDispatchSwitchCase(ofstream &fs, sPacket *packet);
	void WriteDispatchSwitchCaseAsync(ofstream &fs, sPacket *packet);
	void WriteDispatchImpleArg(ofstream &fs, sArg *p, const string &tab
		, const bool isConstructor = false);
	void WriteParsePacketField(ofstream &fs, const string &typeStr
		, const bool isMake = false);
	void WriteLastDispatchSwitchCase(ofstream &fs, sPacket *packet, const string &tab);

	// Write Protocol Code
	bool WriteProtocol(ofstream &fs, sProtocol *protocol);
	void WriteImplPacketList(ofstream &fs, sProtocol *protocol, sPacket *packet);
	void WriteImplPacket(ofstream &fs, sProtocol *protocol, sPacket *packet, sArg*p);
	void WriteImpleArg(ofstream &fs, sArg*p, const string &tab, const string &prefix="");
	void WriteLastImplePacket(ofstream &fs, sProtocol *protocol, sPacket *packet
		, const string &tab, const bool isBinary = false);

	string GetProtocolName(const string &fileName);
	string GetProtocolClassName(const string &protocolName );
	string GetProtocolHandlerClassName(const string &protocolName );
	string GetProtocolDispatcherClassName(const string &protocolName );
	bool ParseTypeString(const string &typeStr, OUT vector<string> &out);

	string g_className; // protocol name + ('_Protocol' | '_ProtocolHandler' | '_Dispatcher')
	string g_protocolId; // protocol name + '_ID'
	string g_protocolName;	// *.prt filename except extends name
	string g_origianlFileName; // protocol name with directory path
	string g_handlerClassName; // protocol handler class name
	string g_srcFolderName = "Src";

	// c++ type convert to TypeScript type mapping, key:C++ Type
	map<string, string> g_typeTable = {
		{"BYTE", "number"},
		{"char", "number"},
		{"bool", "boolean"},
		{"BOOL", "boolean"},
		{"short", "number"},
		{"ushort", "number"},
		{"unsigned short", "number"},
		{"int", "number"},
		{"uint", "number"},
		{"unsigned int", "number"},
		{"int64", "bigint"},
		{"uint64", "bigint"},
		{"__int64", "bigint"},
		{"unsigned __int64", "bigint"},
		{"float", "number"},
		{"double", "number"},
		{"string", "string"},
		{"variant_t", "TypeVariant"},
	};
	// vector type mapping, key:vector argument field
	map<string, string> g_vectorTypeTable = {
		{"BYTE", "Uint8Array | null"},
		{"char", "Int8Array | null"},
		{"bool", "Uint8Array | null"},
		{"BOOL", "Uint8Array | null"},
		{"short", "Int16Array | null"},
		{"ushort", "Uint16Array | null"},
		{"unsigned short", "Uint16Array | null"},
		{"int", "Int32Array | null"},
		{"uint", "Uint32Array | null"},
		{"unsigned int", "Uint32Array | null"},
		{"int64", "BigInt64Array"},
		{"uint64", "BigUint64Array"},
		{"__int64", "BigInt64Array"},
		{"unsigned __int64", "BigUint64Array"},
		{"float", "Float32Array | null"},
		{"double", "Float64Array | null"},
		{"string", "string[]"},
		{"variant_t", "TypeVariant[]"},
	};
	// array type mapping, key:vector argument field
	map<string, string> g_arrayTypeTable = {
		{"BYTE", "number[]"},
		{"char", "number[]"},
		{"bool", "boolean[]"},
		{"BOOL", "boolean[]"},
		{"short", "number[]"},
		{"ushort", "number[]"},
		{"unsigned short", "number[]"},
		{"int", "number[]"},
		{"uint", "number[]"},
		{"unsigned int", "number[]"},
		{"int64", "bigint[]"},
		{"uint64", "bigint[]"},
		{"__int64", "bigint[]"},
		{"unsigned __int64", "bigint[]"},
		{"float", "number[]"},
		{"double", "number[]"},
		{"string", "string[]"},
		{"variant_t", "TypeVariant[]"},
	};
	// map<string, *> type convert to TypeScript type mapping, key:C++ Type
	map<string, string> g_typeMapTable = {
		{"BYTE", "Map<strig, number>"},
		{"char", "Map<strig, number>"},
		{"bool", "Map<strig, boolean>"},
		{"BOOL", "Map<strig, boolean>"},
		{"short", "Map<strig, number>"},
		{"ushort", "Map<strig, number>"},
		{"unsigned short", "Map<strig, number>"},
		{"int", "Map<strig, number>"},
		{"uint", "Map<strig, number>"},
		{"unsigned int", "Map<strig, number>"},
		{"int64", "Map<strig, bigint>"},
		{"uint64", "Map<strig, bigint>"},
		{"__int64", "Map<strig, bigint>"},
		{"unsigned __int64", "Map<strig, bigint>"},
		{"float", "Map<strig, number>"},
		{"double", "Map<strig, number>"},
		{"string", "Map<strig, string>"},
		{"variant_t", "Map<strig, TypeVariant>"},
	};
	// map<string, vector<*>> type mapping, key:vector argument field
	map<string, string> g_mapVectorTypeTable = {
		{"BYTE", "Map<string,Uint8Array> | null"},
		{"char", "Map<string,Int8Array> | null"},
		{"bool", "Map<string,Uint8Array> | null"},
		{"BOOL", "Map<string,Uint8Array> | null"},
		{"short", "Map<string,Int16Array> | null"},
		{"ushort", "Map<string,Uint16Array> | null"},
		{"unsigned short", "Map<string,Uint16Array> | null"},
		{"int", "Map<string,Int32Array> | null"},
		{"uint", "Map<string,Uint32Array> | null"},
		{"unsigned int", "Map<string,Uint32Array> | null"},
		{"int64", "Map<string,BigInt64Array>"},
		{"uint64", "Map<string,BigUint64Array>"},
		{"__int64", "Map<string,BigInt64Array>"},
		{"unsigned __int64", "Map<string,BigUint64Array>"},
		{"float", "Map<string,Float32Array> | null"},
		{"double", "Map<string,Float64Array> | null"},
		{"string", "Map<string,string[]>"},
		{"variant_t", "Map<string,TypeVariant[]>"},
	};
	// packet parse mapping, key:C++ Type
	map<string, string> g_parseTable = {
		{"BYTE", "packet.getUint8()"},
		{"char", "packet.getInt8()"},
		{"bool", "packet.getBool()"},
		{"BOOL", "packet.getBool()"},
		{"short", "packet.getInt16()"},
		{"ushort", "packet.getUint16()"},
		{"unsigned short", "packet.getUint16()"},
		{"int", "packet.getInt32()"},
		{"uint", "packet.getUint32()"},
		{"unsigned int", "packet.getUint32()"},
		{"int64", "packet.getInt64()"},
		{"uint64", "packet.getUint64()"},
		{"__int64", "packet.getInt64()"},
		{"unsigned __int64", "packet.getUint64()"},
		{"float", "packet.getFloat32()"},
		{"double", "packet.getFloat64()"},
		{"string", "packet.getStr()"},
		{"variant_t", "packet.getTypeVariant()"},
	};
	// packet parse mapping (vector type), key:C++ Type
	map<string, string> g_parseVectorTable = {
		{"BYTE", "packet.getUint8Array()"},
		{"char", "packet.getInt8Array()"},
		{"bool", "packet.getUint8Array()"},
		{"BOOL", "packet.getUint8Array()"},
		{"short", "packet.getInt16Array()"},
		{"ushort", "packet.getUint16Array()"},
		{"unsigned short", "packet.getUint16Array()"},
		{"int", "packet.getInt32Array()"},
		{"uint", "packet.getUint32Array()"},
		{"unsigned int", "packet.getUint32Array()"},
		{"int64", "packet.getBigInt64Array()"},
		{"uint64", "packet.getBigUint64Array()"},
		{"__int64", "packet.getBigInt64Array()"},
		{"unsigned __int64", "packet.getBigUint64Array()"},
		{"float", "packet.getFloat32Array()"},
		{"double", "packet.getFloat64Array()"},
		{"string", "packet.getStrArray()"},
		{"variant_t", "packet.getTypeVariantVector()"},
	};
	// packet parse mapping, (map<string,*> type) key:C++ Type
	map<string, string> g_parseMapTable = {
		{"BYTE", "packet.getMapUint8()"},
		{"char", "packet.getMapInt8()"},
		{"bool", "packet.getMapBool()"},
		{"BOOL", "packet.getMapBool()"},
		{"short", "packet.getMapInt16()"},
		{"ushort", "packet.getMapUint16()"},
		{"unsigned short", "packet.getMapUint16()"},
		{"int", "packet.getMapInt32()"},
		{"uint", "packet.getMapUint32()"},
		{"unsigned int", "packet.getMapUint32()"},
		{"int64", "packet.getMapInt64()"},
		{"uint64", "packet.getMapUint64()"},
		{"__int64", "packet.getMapInt64()"},
		{"unsigned __int64", "packet.getMapUint64()"},
		{"float", "packet.getMapFloat32()"},
		{"double", "packet.getMapFloat64()"},
		{"string", "packet.getMapStr()"},
		{"variant_t", "packet.getMapTypeVariant()"},
	};
	// packet parse mapping (map<string, vector<*>> type), key:C++ Type
	map<string, string> g_parseMapVectorTable = {
		{"BYTE", "packet.getMapUint8Array()"},
		{"char", "packet.getMapInt8Array()"},
		{"bool", "packet.getMapUint8Array()"},
		{"BOOL", "packet.getMapUint8Array()"},
		{"short", "packet.getMapInt16Array()"},
		{"ushort", "packet.getMapUint16Array()"},
		{"unsigned short", "packet.getMapUint16Array()"},
		{"int", "packet.getMapInt32Array()"},
		{"uint", "packet.getMapUint32Array()"},
		{"unsigned int", "packet.getMapUint32Array()"},
		{"int64", "packet.getMapBigInt64Array()"},
		{"uint64", "packet.getMapBigUint64Array()"},
		{"__int64", "packet.getMapBigInt64Array()"},
		{"unsigned __int64", "packet.getMapBigUint64Array()"},
		{"float", "packet.getMapFloat32Array()"},
		{"double", "packet.getMapFloat64Array()"},
		{"string", "packet.getMapStrArray()"},
		{"variant_t", "packet.getMapTypeVariantVector()"},
	};

	// packet make mapping, key:C++ Type
	map<string, string> g_makeTable = {
		{"BYTE", "packet.pushUint8("},
		{"char", "packet.pushInt8("},
		{"bool", "packet.pushBool("},
		{"BOOL", "packet.pushBool("},
		{"short", "packet.pushInt16("},
		{"ushort", "packet.pushUint16("},
		{"unsigned short", "packet.pushUint16("},
		{"int", "packet.pushInt32("},
		{"uint", "packet.pushUint32("},
		{"unsigned int", "packet.pushUint32("},
		{"int64", "packet.pushInt64("},
		{"uint64", "packet.pushUint64("},
		{"__int64", "packet.pushInt64("},
		{"unsigned __int64", "packet.pushUint64("},
		{"float", "packet.pushFloat32("},
		{"double", "packet.pushFloat64("},
		{"string", "packet.pushStr("},
		{"variant_t", "packet.pushTypeVariant("},
	};
	// packet make mapping (vector type), key:C++ Type
	map<string, string> g_makeVectorTable = {
		{"BYTE", "packet.pushUint8Array("},
		{"char", "packet.pushInt8Array("},
		{"bool", "packet.pushBoolArray("},
		{"BOOL", "packet.pushBoolArray("},
		{"short", "packet.pushInt16Array("},
		{"ushort", "packet.pushUint16Array("},
		{"unsigned short", "packet.pushUint16Array("},
		{"int", "packet.pushInt32Array("},
		{"uint", "packet.pushUint32Array("},
		{"unsigned int", "packet.pushUint32Array("},
		{"int64", "packet.pushInt64Array("},
		{"uint64", "packet.pushUint64Array("},
		{"__int64", "packet.pushInt64Array("},
		{"unsigned __int64", "packet.pushUint64Array("},
		{"float", "packet.pushFloat32Array("},
		{"double", "packet.pushFloat64Array("},
		{"string", "packet.pushStrArray("},
		{"variant_t", "packet.pushTypeVariantVector("},
	};
	// packet make mapping (map<string, *> type), key:C++ Type
	map<string, string> g_makeMapTable = {
		{"BYTE", "packet.pushMapUint8("},
		{"char", "packet.pushMapInt8("},
		{"bool", "packet.pushMapBool("},
		{"BOOL", "packet.pushMapBool("},
		{"short", "packet.pushMapInt16("},
		{"ushort", "packet.pushMapUint16("},
		{"unsigned short", "packet.pushMapUint16("},
		{"int", "packet.pushMapInt32("},
		{"uint", "packet.pushMapUint32("},
		{"unsigned int", "packet.pushMapUint32("},
		{"int64", "packet.pushMapInt64("},
		{"uint64", "packet.pushMapUint64("},
		{"__int64", "packet.pushMapInt64("},
		{"unsigned __int64", "packet.pushMapUint64("},
		{"float", "packet.pushMapFloat32("},
		{"double", "packet.pushMapFloat64("},
		{"string", "packet.pushMapStr("},
		{"variant_t", "packet.pushMapTypeVariant("},
	};
	// packet make mapping (map<string, vector<*>> type), key:C++ Type
	map<string, string> g_makeMapVectorTable = {
		{"BYTE", "packet.pushMapUint8Array("},
		{"char", "packet.pushMapInt8Array("},
		{"bool", "packet.pushMapBoolArray("},
		{"BOOL", "packet.pushMapBoolArray("},
		{"short", "packet.pushMapInt16Array("},
		{"ushort", "packet.pushMapUint16Array("},
		{"unsigned short", "packet.pushMapUint16Array("},
		{"int", "packet.pushMapInt32Array("},
		{"uint", "packet.pushMapUint32Array("},
		{"unsigned int", "packet.pushMapUint32Array("},
		{"int64", "packet.pushMapInt64Array("},
		{"uint64", "packet.pushMapUint64Array("},
		{"__int64", "packet.pushMapInt64Array("},
		{"unsigned __int64", "packet.pushMapUint64Array("},
		{"float", "packet.pushMapFloat32Array("},
		{"double", "packet.pushMapFloat64Array("},
		{"string", "packet.pushMapStrArray("},
		{"variant_t", "packet.pushMapTypeVariantVector("},
	};
	// custom types
	map<string, sType*> g_customTypes; // reference
	bool g_isAsync = false; // async protocol handler module?
}

using namespace compiler2;


//------------------------------------------------------------------------
// generate TypeScript code
// generate Protocol, Handler, Dispatcher, Packet Data class
//------------------------------------------------------------------------
bool compiler2::WriteProtocolCode(const string &protocolFileName, sProtocol *protocol
	, sType *type
	, const bool isAsyncModule //= false
)
{
	const string fileName = common::GetFileNameExceptExt(protocolFileName);
	const string path = common::GetFilePathExceptFileName(protocolFileName);
	const string folder = (path.empty()) ? g_srcFolderName : path + "\\" + g_srcFolderName;
	_mkdir(folder.c_str());
	g_origianlFileName = folder + "\\" + fileName;
	if (isAsyncModule)
		g_origianlFileName += "Async";

	if (protocol->format != "json")
		return true; // only json format

	g_protocolName = GetProtocolName(protocolFileName);
	if (isAsyncModule)
		g_protocolName += "Async";

	g_isAsync = isAsyncModule;

	const string tsFileName = g_origianlFileName + "_handler.ts";

	// update custom type table
	{
		g_customTypes.clear();
		sType *p = type;
		while (p)
		{
			g_customTypes[p->name] = p;
			p = p->next;
		}
	}

	ofstream fs;
	fs.open(tsFileName.c_str());
	if (!fs.is_open())
		return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << fileName << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "import TypeVariant from \"../common/variant\";\n";
	fs << "import { Network } from \"../network/network\";\n";
	fs << "\n";

	fs << "export namespace " << g_protocolName << " {\n";

	WriteCustomStruct(fs, type);
	WriteProtocolData(fs, protocol);
	WriteDispatcher(fs, protocol);
	WriteProtocol(fs, protocol);
	WriteHandler(fs, protocol);

	fs << "}\n\n";

	return true;
}


//------------------------------------------------------------------------
// return filename except extends name
//------------------------------------------------------------------------
string compiler2::GetProtocolName(const string &fileName)
{
	return common::GetFileNameExceptExt(fileName);
}


//------------------------------------------------------------------------
// return protocol class name, protocol name + _Protocol
//------------------------------------------------------------------------
string compiler2::GetProtocolClassName(const string &protocolName )
{
	return protocolName + "_Protocol";
}


//------------------------------------------------------------------------
// return protocol handler class name, protocol name + _ProtocolHandler
//------------------------------------------------------------------------
string compiler2::GetProtocolHandlerClassName(const string &protocolName )
{
	return protocolName + "_ProtocolHandler";
}


//------------------------------------------------------------------------
// return protocol dispatcher class name, protocol name + _Dispatcher
//------------------------------------------------------------------------
string compiler2::GetProtocolDispatcherClassName(const string &protocolName )
{
	return protocolName + "_Dispatcher";
}


// convert type string to string array
// type string -> bool | int | string | float | vector | map
// vector<type string>, map<type string, type string>
// ex) map<string, vector<string>>
//      - return: [map, string, vector, string]
bool compiler2::ParseTypeString(const string &typeStr, OUT vector<string> &out)
{
	const int idx0 = typeStr.find('>');
	const int idx1 = typeStr.find('<');
	const int idx = ((idx0 < 0) && (idx1 < 0)) ? -1 :
		(((idx0 < 0) || (idx1 < 0)) ? max(idx0, idx1)
			: min(idx0, idx1));

	if (idx < 0)
	{
		vector<string> strs;
		common::tokenizer(typeStr, ",", "", strs);
		for (auto &s : strs)
		{
			common::trim(s);
			if (s.empty())
				continue;
			out.push_back(s);
		}
	}
	else
	{
		const string tstr = common::trim2(typeStr.substr(0, idx));
		ParseTypeString(tstr, out);
		const string str = common::trim2(typeStr.substr(idx + 1));
		ParseTypeString(str, out);
	}
	return !out.empty();
}


//------------------------------------------------------------------------
// in Protocol
// generate Protocol code
//------------------------------------------------------------------------
bool compiler2::WriteProtocol(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolClassName(protocol->name);
	fs << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// " << g_protocolName << " " << protocol->name << " Protocol \n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "export class " << g_className << " extends Network.Protocol {\n";
	fs << "\t constructor() { super() }\n";
	fs << "\n";
	WriteImplPacketList(fs, protocol, protocol->packet);
	fs << "}\n";
	fs << endl;

	return WriteProtocol(fs, protocol->next);
}


// WriteDataHeader
 bool compiler2::WriteProtocolData(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;
	fs << endl;
	fs << "\t// " << protocol->name << " Protocol Packet Data\n";
	WritePacketStruct(fs, protocol->packet);
	fs << endl;
	return WriteProtocolData(fs, protocol->next);
}


// Write Custom Structure
bool compiler2::WriteCustomStruct(ofstream &fs, sType *type)
{
	if (!type) return true;

	// declare custom type
	fs << "\texport type " << type->name << " = {" << endl;
	WritePacketField(fs, type->vars);
	fs << "\t}" << endl;

	// implement custom type parse
	fs << "\tfunction Parse_" << type->name << "(packet: Network.Packet) : "
		<< type->name << " {\n";
	fs << "\t\treturn {\n";
	WriteDispatchImpleArg(fs, type->vars, "\t\t\t", true);
	fs << "\t\t}\n";
	fs << "\t}\n";

	// implement custom type vector parse
	fs << "\tfunction Parse_" << type->name << "Vector(packet: Network.Packet) : "
		<< type->name << "[] {\n";

	fs << "\t\tconst size = packet.getUint32()\n";
	fs << "\t\tif (size == 0) return []\n";
	fs << "\t\tlet ar: " << type->name << "[] = []\n";
	fs << "\t\tfor(let i=0; i < size; ++i)\n";
	fs << "\t\t\tar.push(";
	WriteParsePacketField(fs, type->name);
	fs << ")\n";
	fs << "\t\treturn ar\n";
	fs << "\t}\n";

	// implement custom type make
	fs << "\tfunction Make_" << type->name << "(packet: Network.Packet, data: "
		<< type->name << ") {\n";
	WriteImpleArg(fs, type->vars, "\t\t", "data.");
	fs << "\t}\n";

	// implement custom type vector make
	fs << "\tfunction Make_" << type->name << "Vector(packet: Network.Packet, data: "
		<< type->name << "[]) {\n";
	fs << "\t}\n";

	return WriteCustomStruct(fs, type->next);
}


 // Write Packet data structure
bool compiler2::WritePacketStruct(ofstream &fs, sPacket *packet)
{
	if (!packet) return true;
	fs << "\texport type " << packet->name << "_Packet = {" << endl;
	WritePacketField(fs, packet->argList);
	fs << "\t}" << endl;
	return WritePacketStruct(fs, packet->next);
}


// Write Packet Field
// isDecl: declare state?
void compiler2::WritePacketField(ofstream &fs, sArg *arg
	, const bool isNewLine //=true
	, const bool isDecl //=false
)
{
	if (!arg) return;

	// convert C++ type to TypeScript type
	string typeStr = "notdef";
	auto it1 = g_typeTable.find(arg->var->type);
	if (g_typeTable.end() != it1)
	{
		typeStr = it1->second;
	}
	else
	{
		vector<string> types;
		ParseTypeString(arg->var->type, types);
		if (types.size() > 1)
		{
			if ("vector" == types[0])
			{
				map<string, string> *typeTable =
					(isDecl) ? &g_arrayTypeTable : &g_vectorTypeTable;

				auto it2 = typeTable->find(types[1]);
				if (typeTable->end() != it2)
				{
					typeStr = it2->second;
				}
				else
				{
					vector<string> toks;
					common::tokenizer(types[1], "::", "", toks);
					if (!toks.empty())
						typeStr = toks.back() + "[]";
				}
			}
			else if (("map" == types[0]) && (types.size() >= 3))
			{
				if (types[2] == "vector") // map value type
				{
					map<string, string>* typeTable = &g_mapVectorTypeTable;
					const string type = (types.size() >= 4) ? types[3] : ""; // vector type

					auto it2 = typeTable->find(type);
					if (typeTable->end() != it2)
					{
						typeStr = it2->second;
					}
					else
					{
						vector<string> toks;
						common::tokenizer(type, "::", "", toks);
						if (!toks.empty())
							typeStr = toks.back() + "[]";
					}
				}
				else
				{
					map<string, string>* typeTable = &g_typeMapTable;

					auto it2 = typeTable->find(types[2]);
					if (typeTable->end() != it2)
					{
						typeStr = it2->second;
					}
					else
					{
						vector<string> toks;
						common::tokenizer(types[2], "::", "", toks);
						if (!toks.empty())
							typeStr = toks.back() + "[]";
					}
				}
			}
		}
		else
		{
			// find custom type
			vector<string> toks;
			common::tokenizer(arg->var->type, "::", "", toks);
			if (!toks.empty())
			{
				const string &type = toks.back();
				auto it2 = g_customTypes.find(type);
				if (g_customTypes.end() != it2)
				{
					typeStr = type;
				}
			}
		}
	}

	if (isNewLine)
		fs << "\t\t";
	fs << arg->var->var << ": " << typeStr << ", ";
	if (isNewLine)
		fs << endl;

	WritePacketField(fs, arg->next, isNewLine, isDecl);
}


//------------------------------------------------------------------------
// in Protocol Handler
// generate protocol handler interface code
//------------------------------------------------------------------------
bool compiler2::WriteHandler(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolHandlerClassName(protocol->name);
	fs << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// " << g_protocolName << " " << protocol->name << " Protocol Handler\n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "export class " << g_className << " extends Network.Handler {\n";
	fs << "\t constructor() { super() } \n";
	fs << "\n";
	WriteDeclPacketList(fs, protocol->packet);
	fs << "}\n";
	fs << endl;
	return WriteHandler(fs, protocol->next);
}


//------------------------------------------------------------------------
// generate Protocol Dispatcher code
//------------------------------------------------------------------------
bool compiler2::WriteDispatcher(ofstream &fs, sProtocol *protocol)
{
	if (!protocol) return true;

	g_className = GetProtocolDispatcherClassName(protocol->name);
	fs << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// " << g_protocolName << " " << protocol->name << " Protocol Dispatcher\n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "export class " << g_className << " extends Network.Dispatcher {\n";
	fs << "\t constructor() {\n";
	fs << "\t\tsuper(" << protocol->number << ")\n";
	fs << "\t}\n";
	if (g_isAsync)
		WriteProtocolDispatchAsyncFunc(fs, protocol);
	else
		WriteProtocolDispatchFunc(fs, protocol);	
	fs << "}\n";
	fs << endl;
	return WriteDispatcher(fs, protocol->next);
}


//------------------------------------------------------------------------
// in Protocol.h
// generate packet function list code
//------------------------------------------------------------------------
void compiler2::WriteDeclPacketList(ofstream &fs, sPacket *packet)
{
	if (!packet) return;

	fs << "\t" << packet->name << " = (";
	fs << "packet: " << g_protocolName << "." << packet->name << "_Packet";
	fs << ") => { }"; // Handler header file
	fs << endl;

	WriteDeclPacketList(fs, packet->next);
}


//------------------------------------------------------------------------
// in Protocol.cpp
// generate packet send function code
//------------------------------------------------------------------------
void compiler2::WriteImplPacketList(ofstream &fs, sProtocol *protocol
	, sPacket *packet)
{
	if (!packet) return;
	const string tab = "\t";
	fs << tab << "// Protocol: " << packet->name << endl;
	fs << tab << packet->name << "(";
	WriteDeclPacketFirstArg(fs, packet->argList);
	fs << ") {\n";
	WriteImplPacket(fs, protocol, packet, packet->argList);
	fs << tab << "}\n";
	fs << tab << "\n";
	WriteImplPacketList(fs, protocol, packet->next);
}


//------------------------------------------------------------------------
// generate make packet argument code
//------------------------------------------------------------------------
void compiler2::WriteDeclPacketFirstArg(ofstream &fs, sArg*p)
{
	fs << "isBinary: boolean, ";
	WritePacketField(fs, p, false, true);
}


//------------------------------------------------------------------------
// in Protocol Class
// generate make packet and send code
//------------------------------------------------------------------------
void compiler2::WriteImplPacket(ofstream &fs, sProtocol *protocol
	, sPacket *packet, sArg *p)
{
	const string tab = "\t\t";

	//fs << tab << "if (!this.ws)\n";
	//fs << tab << "\treturn\n";

	// binary packing
	fs << tab << "if (isBinary) { // binary send?\n";
	fs << tab << "\tlet packet = new Network.Packet(512)\n";
	WriteImpleArg(fs, p, tab + "\t");
	WriteLastImplePacket(fs, protocol, packet, tab + "\t", true);
	fs << tab << "} else { // json string send?\n";
	// json object packing
	fs << tab << "\tconst packet = {\n";
	{
		sArg *p = packet->argList;
		while (p)
		{
			fs << tab << "\t\t" << p->var->var << ",\n";
			p = p->next;
		}
	}
	fs << tab << "\t}\n";
	WriteLastImplePacket(fs, protocol, packet, tab + "\t");
	fs << tab << "}\n";
}


//------------------------------------------------------------------------
// in Protocol class
// generate make packet code
//------------------------------------------------------------------------
void compiler2::WriteImpleArg(ofstream &fs, sArg *p, const string &tab
	, const string &prefix //= ""
)
{
	if (!p) return;
	fs << tab;
	WriteParsePacketField(fs, p->var->type, true);
	fs << prefix << p->var->var << ")";
	fs << "\n";
	WriteImpleArg(fs, p->next, tab, prefix);
}


//------------------------------------------------------------------------
// in Protocol class
// generate send packet code 
// isBinary: binary packet send?
//------------------------------------------------------------------------
void compiler2::WriteLastImplePacket(ofstream &fs, sProtocol *protocol
	, sPacket *packet, const string &tab
	, const bool isBinary //= false
)
{
	if (isBinary)
	{
		fs << tab << "this.node?.sendPacketBinary("
			<< protocol->number << ", " << packet->packetId
			<< ", packet.buff, packet.offset)\n";

		//fs << tab << "Network.sendPacketBinary(this.ws, "
		//	<< protocol->number << ", " << packet->packetId 
		//	<< ", packet.buff, packet.offset)\n";
	}
	else
	{
		fs << tab << "this.node?.sendPacket("
			<< protocol->number << ", " << packet->packetId << ", packet)\n";

		//fs << tab << "Network.sendPacket(this.ws, "
		//	<< protocol->number << ", " << packet->packetId << ", packet)\n";
	}
}


//------------------------------------------------------------------------
// generate Dispatcher::Dispatch() code
//------------------------------------------------------------------------
void compiler2::WriteProtocolDispatchFunc(ofstream &fs, sProtocol *protocol)
{
	g_handlerClassName = GetProtocolHandlerClassName(protocol->name);

	fs << "\tdispatch(ws: WebSocket, packet: Network.Packet, handlers: Network.Handler[]) {\n";

	const string tab = "\t\t";
	fs << tab << "if (!packet.buff) return\n";
	fs << tab << "packet.init()\n";
	fs << tab << "// packet format\n";
	fs << tab << "// | protocol id (4) | packet id (4) | packet length (4) | option (4) |\n";
	fs << tab << "const protocolId = packet.getUint32()\n";
	fs << tab << "const packetId = packet.getUint32()\n";
	fs << tab << "const packetLength = packet.getUint32()\n";
	fs << tab << "const option = packet.getUint32()\n";
	fs << "\n";
	fs << tab << "switch (packetId) {\n";
	WriteDispatchSwitchCase(fs, protocol->packet);
	fs << tab << "\tdefault:\n";
		fs << tab << "\t\tconsole.log(`not found packet ${protocolId}, ${packetId}`)\n";
		fs << tab << "\t\tbreak;\n";
	fs << tab << "}//~switch\n";
	fs << "\t}//~dispatch()\n";
}


//------------------------------------------------------------------------
// generate Dispatcher::Dispatch() code
//------------------------------------------------------------------------
void compiler2::WriteProtocolDispatchAsyncFunc(ofstream &fs, sProtocol *protocol)
{
	g_handlerClassName = GetProtocolHandlerClassName(protocol->name);

	fs << "\tdispatchAsync(protocolId: number, packetId: number, parsePacket: any, handlers: Network.Handler[]) {\n";

	const string tab = "\t\t";
	fs << tab << "switch (packetId) {\n";
	WriteDispatchSwitchCaseAsync(fs, protocol->packet);
	fs << tab << "\tdefault:\n";
	fs << tab << "\t\tconsole.log(`not found packet ${protocolId}, ${packetId}`)\n";
	fs << tab << "\t\tbreak;\n";
	fs << tab << "}//~switch\n";
	fs << "\t}//~dispatch()\n";
}


//------------------------------------------------------------------------
// generate Dispatcher switch case code
//------------------------------------------------------------------------
void compiler2::WriteDispatchSwitchCase(ofstream &fs, sPacket *packet)
{
	if (!packet) return;

	fs << "\t\t\tcase " << packet->packetId << ": // " << packet->name << "\n";
	fs << "\t\t\t\t{\n";

	const string tab = "\t\t\t\t\t";

	// binary parsing
	fs << tab << "if (option == 1) { // binary?\n";
		WriteDispatchImpleArg(fs, packet->argList, tab + "\t");
		fs << tab << "\tconst parsePacket: " << packet->name << "_Packet = {\n";
		{
			sArg *p = packet->argList;
			while (p)
			{
				fs << tab << "\t\t" << p->var->var << ",\n";
				p = p->next;
			}
		}
		fs << tab << "\t}\n";
		WriteLastDispatchSwitchCase(fs, packet, tab + "\t");

	fs << tab << "} else { // json?\n";
	// json string parsing
	fs << tab << "\tconst parsePacket: " << packet->name << "_Packet = \n";
	fs << tab << "\t\tJSON.parse(packet.getStr())\n";
	WriteLastDispatchSwitchCase(fs, packet, tab + "\t");
	fs << tab << "}\n";

	fs << "\t\t\t\t}\n";
	fs << "\t\t\t\tbreak;\n";
	fs << "\n";

	WriteDispatchSwitchCase(fs, packet->next);
}


//------------------------------------------------------------------------
// generate Dispatcher switch case code
//------------------------------------------------------------------------
void compiler2::WriteDispatchSwitchCaseAsync(ofstream &fs, sPacket *packet)
{
	if (!packet) return;

	fs << "\t\t\tcase " << packet->packetId << ": // " << packet->name << "\n";
	fs << "\t\t\t\t{\n";

	const string tab = "\t\t\t\t";
	WriteLastDispatchSwitchCase(fs, packet, tab + "\t");

	fs << "\t\t\t\t}\n";
	fs << "\t\t\t\tbreak;\n";
	fs << "\n";

	WriteDispatchSwitchCaseAsync(fs, packet->next);
}


//------------------------------------------------------------------------
// in dispatcher class
// read packet data and save protocol data structure
// isConstructor: is constructor type implement?
//------------------------------------------------------------------------
void compiler2::WriteDispatchImpleArg(ofstream &fs, sArg*p
	, const string &tab
	, const bool isConstructor //=false
)
{
	if (!p) return;
	if (isConstructor)
	{
		fs << tab << p->var->var << ": ";
		WriteParsePacketField(fs, p->var->type);
		fs << ",";
	}
	else
	{
		fs << tab << "const " << p->var->var << " = ";
		WriteParsePacketField(fs, p->var->type);
	}
	fs << "\n";
	WriteDispatchImpleArg(fs, p->next, tab, isConstructor);
}


//------------------------------------------------------------------------
// in dispatcher class
// generate parse/make packet field code
// typeStr: type string
// isMake: parse or make packet?
//------------------------------------------------------------------------
void compiler2::WriteParsePacketField(ofstream &fs, const string &typeStr
	, const bool isMake //=false
)
{
	map<string, string> *table = isMake? &g_makeTable : &g_parseTable;
	map<string, string> *vectorTable = isMake? &g_makeVectorTable : &g_parseVectorTable;

	string code = "0"; // default
	auto it1 = table->find(typeStr);
	if (table->end() != it1)
	{
		code = it1->second;
	}
	else
	{
		vector<string> types;
		ParseTypeString(typeStr, types);

		if (types.size() > 1)
		{
			if (types[0] == "vector")
			{
				auto it2 = vectorTable->find(types[1]);
				if (vectorTable->end() != it2)
				{
					code = it2->second;
				}
				else
				{
					// custom type?
					vector<string> toks;
					common::tokenizer(types[1], "::", "", toks);
					auto it3 = g_customTypes.find(toks.back());
					if (g_customTypes.end() != it3)
					{
						sType *type = it3->second;
						if (isMake)
							code = string("Make_") + type->name + "Vector(packet,";
						else
							code = string("Parse_") + type->name + "Vector(packet)";
					}
				}
			}
			else if (types[0] == "array")
			{
				// custom type?
				vector<string> toks;
				common::tokenizer(types[1], "::", "", toks);
				auto it3 = g_customTypes.find(toks.back());
				if (g_customTypes.end() != it3)
				{
					sType *type = it3->second;
					if (isMake)
						code = string("Make_") + type->name + "Array(packet,";
					else
						code = string("Parse_") + type->name + "Array(packet)";
				}
				else
				{
					code = "[]"; // empty array, todo: parse array
				}
			}
			else if ((types[0] == "map") && (types.size() >= 3))
			{
				if (types[2] == "vector") // map value type is vector?
				{
					map<string, string>* mapTable = isMake ? &g_makeMapVectorTable : &g_parseMapVectorTable;
					const string type = (types.size() >= 4) ? types[3] : "";

					auto it2 = mapTable->find(type); // vector type
					if (mapTable->end() != it2)
					{
						code = it2->second;
					}
					else
					{
						// custom type?
						vector<string> toks;
						common::tokenizer(type, "::", "", toks);
						auto it3 = g_customTypes.find(toks.back());
						if (g_customTypes.end() != it3)
						{
							sType* type = it3->second;
							if (isMake)
								code = string("Make_") + type->name + "MapVector(packet,";
							else
								code = string("Parse_") + type->name + "MapVector(packet)";
						}
					}
				}
				else
				{
					map<string, string>* mapTable = isMake ? &g_makeMapTable : &g_parseMapTable;

					auto it2 = mapTable->find(types[2]); // value type
					if (mapTable->end() != it2)
					{
						code = it2->second;
					}
					else
					{
						// custom type?
						vector<string> toks;
						common::tokenizer(types[2], "::", "", toks);
						auto it3 = g_customTypes.find(toks.back());
						if (g_customTypes.end() != it3)
						{
							sType* type = it3->second;
							if (isMake)
								code = string("Make_") + type->name + "Map(packet,";
							else
								code = string("Parse_") + type->name + "Map(packet)";
						}
					}
				}
			}
		}
		else if (!types.empty())
		{
			// custom type parsing?
			vector<string> toks;
			common::tokenizer(types[0], "::", "", toks);
			auto it2 = g_customTypes.find(toks.back());
			if (g_customTypes.end() != it2)
			{
				sType *type = it2->second;
				if (isMake)
					code = string("Make_") + type->name + "(packet,";
				else
					code = string("Parse_") + type->name + "(packet)";
			}
		}
	}

	fs << code;
}


//------------------------------------------------------------------------
// in dispatcher class
// generate call handler function code
// ex) SEND_HANDLER(c2s_ProtocolHandler, prtHandler, SpawnRobot(data));
//------------------------------------------------------------------------
void compiler2::WriteLastDispatchSwitchCase(ofstream &fs, sPacket *packet, const string &tab)
{
	fs << tab << "handlers.forEach(handler => {\n";
	fs << tab << "\tif (handler instanceof " << g_handlerClassName << ")\n";
	fs << tab << "\t\thandler." << packet->name << "(parsePacket)\n";
	fs << tab << "})\n";
}
