//------------------------------------------------------------------------
// Name:    ProtocolScanner.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// Protocol Scanner
//
// 2019-03-31, jjuiddong
//	- check UTF-8 with BOM
//		- https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
//		- Notepad normally uses ANSI encoding, so if it reads the file as UTF - 8 
//		  then it has to guess the encoding based on the data in the file.
//		  If you save a file as UTF - 8, Notepad will put the BOM(byte order mark) 
//		  EF BB BF at the beginning of the file.
//
//
// 2021-08-22
//	- refactoring
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

	public:
		bool LoadFile( const string &fileName, bool isTrace=false );
		bool LoadPackageFile( BYTE *fileMem, int fileSize );

		eTokentype GetToken();
		eTokentype GetTokenQ( int nIdx );
		char* GetTokenStringQ( int nIdx );
		char* CopyTokenStringQ( int nIdx );
		int GetLineNo() { return m_lineNo; }
		void CheckUTF8WithBOM();
		bool IsEnd();
		void Clear();


	protected:
		void Init();
		char GetNextChar();
		void UngetNextChar();
		eTokentype _GetToken( char *pToken );
		bool GetString(char *receiveBuffer, int maxBufferLength);


	public:
		enum { MAX_QSIZE = 5, MAX_BUFF = 256, MAX_TOKENLEN = 256 };
		enum eStateType {
			START, INASSIGN, INCOMMENT, INMULTI_COMMENT
			, OUTMULTI_COMMENT, INNUM, INID, INSTR, INDIV, INEQ, INNEQ, INOR, INAND
			, INLTEQ, INRTEQ, INARROW, INSCOPE, DONE
		};

		struct sTokDat
		{
			string str;
			eTokentype tok;
		};

		BYTE *m_fileMem;
		int m_currentMemPoint;
		int m_memSize;

		int m_lineNo;
		int m_linePos;
		int m_bufSize;
		char m_buf[MAX_BUFF];
		std::deque<sTokDat> m_tokQ;
		bool m_isTrace;
	};

}
