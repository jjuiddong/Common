//------------------------------------------------------------------------
// Name:    ProtocolParser.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// Protocol Parser
//
// 2019-02-26
//	- add command {packetid : number} 
//	- packetid = packet name hashcode
//
// 2020-11-11
//	- protocol format
//	- binary, ascii, json
//	- add protocol format command
//		ex) protocol c2s 1000  <-- binary format default
//		ex) protocol c2s 1000 binary
//		ex) protocol c2s 1000 ascii
//		ex) protocol c2s 1000 json
//
// 2021-08-22
//	- refactoring
//	- custom struture type parsing
//	- ex)
//			type sSyncSymbol
//			{
//				string scope;
//				string name;
//				variant_t var;
//			}
//
//------------------------------------------------------------------------
#pragma once

#include "ProtocolScanner.h"


namespace network2
{

	class cProtocolScanner;
	class cProtocolParser
	{
	public:
		cProtocolParser();
		virtual ~cProtocolParser();

		sStmt* Parse( const string &fileName, bool isTrace=false, bool isLog=true );
		bool IsError() { return m_isError; }
		void SetAutoRemove(bool isAutoRemove) { m_isAutoRemove = isAutoRemove; }


	private:
		bool Match( eTokentype t );
		void SyntaxError( const char *szMsg, ... );

		// expr -> stmt_list
		// stmt_list -> (protocol | type_stmt)*
		// protocol -> 'protocol' id number [id] '{' packet_list '}'
		// packet_list -> (packet)*
		// packet -> [ packetid_stmt ] id '(' arg_list ')' semicolon
		// type_stmt -> type id '{' decl_list '}'
		// decl_list -> decl [ decl_list ]
		// decl -> arg semicolon
		// arg_list -> [arg (',' arg)*]
		// arg -> type
		// type -> type_sub (var)?
		// type_sub -> id '<' type_sub '>'
		//			| id::id
		//			| id
		// var -> '*' id (index)?
		//	    | '&' id (index)?
		//		| id (index)?
		//	    | '*'
		//		| '&'
		// index -> '[' (number)? ']'

		sStmt* stmt_list();
		sProtocol* protocol();
		sType* type_stmt();
		sPacket* packet_list();
		sPacket* packet();
		sArg* arg_list();
		sArg* decl_list();
		sArg* arg();
		sArg* decl();
		sTypeVar* type();
		string type_sub();
		string var();
		string index();
		string number();
		int num();
		string id();


	protected:
		string m_fileName;
		sStmt *m_stmts; // statement linked list
		cProtocolScanner m_scan;
		eTokentype m_token;
		bool m_isTrace;
		bool m_isError;
		bool m_isAutoRemove;
	};

}
