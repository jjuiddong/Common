//------------------------------------------------------------------------
// Name:    ProtocolScanner.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// 프로토콜 스캐너
//
// 2019-03-31, jjuiddong
//	- check UTF-8 with BOM
//		- https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
//		- Notepad normally uses ANSI encoding, so if it reads the file as UTF - 8 
//		  then it has to guess the encoding based on the data in the file.
//		  If you save a file as UTF - 8, Notepad will put the BOM(byte order mark) 
//		  EF BB BF at the beginning of the file.
//
//------------------------------------------------------------------------
#pragma once

#include "ProtocolDefine.h"

namespace network2
{
	class cProtocolScanner
	{
	public:
		cProtocolScanner();
		virtual ~cProtocolScanner();

	protected:
		enum { MAX_QSIZE=5, MAX_BUFF=256, MAX_TOKENLEN=256 };
		enum StateType { START, INASSIGN, INCOMMENT, INMULTI_COMMENT
			, OUTMULTI_COMMENT, INNUM, INID, INSTR, INDIV, INEQ, INNEQ, INOR, INAND
			, INLTEQ, INRTEQ, INARROW, INSCOPE, DONE };

		typedef struct _tagSTokDat
		{
			std::string str;
			Tokentype tok;
		} STokDat;

		BYTE *m_pFileMem;
		int m_pCurrentMemPoint;
		int m_nMemSize;

		int m_nLineNo;
		int m_nLinePos;
		int m_nBufSize;
		char m_Buf[ MAX_BUFF];
		std::deque<STokDat> m_TokQ;
		BOOL m_bTrace;


	public:
		BOOL LoadFile( const char *szFileName, BOOL bTrace=FALSE );
		BOOL LoadPackageFile( BYTE *pFileMem, int nFileSize );

		Tokentype GetToken();
		Tokentype GetTokenQ( int nIdx );
		char* GetTokenStringQ( int nIdx );
		char* CopyTokenStringQ( int nIdx );
		int GetLineNo() { return m_nLineNo; }
		void CheckUTF8WithBOM();
		BOOL IsEnd();
		void Clear();


	protected:
		void Init();
		char GetNextChar();
		void UngetNextChar();
		Tokentype _GetToken( char *pToken );

		BOOL GetString(char *receiveBuffer, int maxBufferLength);
	};
}
