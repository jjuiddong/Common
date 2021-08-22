//------------------------------------------------------------------------
// Name:    ScriptDefine.h
// Author:  jjuiddong
// Date:    1/3/2013
// 
// protocol compiler definition
//	- parse tree
//
// 2021-08-22
//	- refactoring
//	- parse custom type structure
//
//------------------------------------------------------------------------
#pragma once


namespace network2
{
	enum
	{
		MAX_STRING = 256,
		MAX_ARGUMENT = 10,
	};

	enum eTokentype
	{
		_ERROR, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA, COLON, SEMICOLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, REF, ARROW,
		LT/* < */, RT/* > */, LTEQ/* <= */, RTEQ/* >= */, NEQ/* != */, EQ/* == */, OR/* || */, AND/* && */, NEG/* ! */, SCOPE/*::*/,
		EVENT, UIEVENT, CUSTOMEVENT, IF, ELSE, WHILE, FUNCTION, ARG_IN, PROTOCOL, TYPE,
	};

	enum eNodeKind { Stmt, Exp };
	enum eKind
	{
		ProgramK, TutorialK, RoleTutorialK, TrainingK, PracticeK, StatementK, AIK, EventK, UIEventK, 
		CustomEventK, SuccessK, FailK, AssignK,
		ScriptWaitK, ScriptEndK, ScriptExitK, // statement
		TypeK, ConditionOpK, ConditionTreeK, OpK, IdK, FuncK, ParamK, ConstIntK, ConstFloatK, ConstStrK, CallK, // exp
	};

	enum { MAXCHILD=8, };

	enum eCONDITION_OP { OP_NONE, OP_AND, OP_OR, OP_NEG, OP_LT, OP_RT, OP_LTEQ, OP_RTEQ, OP_EQ, OP_NEQ };


	struct sTypeVar
	{
		string type; // type name
		string var; // variable name
	};

	struct sArg
	{
		sTypeVar *var;
		sArg *next;
	};

	struct sPacket
	{
		string name;
		uint packetId;
		sArg *argList;
		sPacket *next;
	};

	struct sProtocol
	{
		string name;
		int number;
		string format; // binary, ascii, json
		sPacket *packet;
		sProtocol *next;
	};

	struct sType
	{
		string name;
		sArg *vars;
	};

	struct sStmt
	{
		sProtocol *protocol;
		sType *type;
		sStmt *next;
		sStmt() : protocol(nullptr), type(nullptr) {}
	};

	// Release Protocol Parser Tree
	void ReleaseStmt(sStmt *p);
	void ReleaseProtocol(sProtocol *p);
	void ReleaseProtocolOnly(sProtocol *p);
	void ReleaseProtocolOnly(sStmt *p);
	void ReleaseType(sType *p);
	void ReleasePacket(sPacket *p);
	void ReleaseCurrentPacket(sPacket *p);
	void ReleaseArg(sArg *p);
	
	// Functions
	void PrintToken( eTokentype token, char *szTokenString );

	void GetPacketElement(const ePacketFormat format
		, const string &typeStr, cPacket &packet, OUT _variant_t &var);
	//int GetPacketID(sRmi *rmi, sPacket *packet);
	string Packet2String(const cPacket &packet, sPacket *protocol);

}
