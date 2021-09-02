
#include "stdafx.h"
#include "ProtocolScanner.h"

using namespace network2;

// lookup table of reserved words
struct sReservedWord
{
	const char *str;
	eTokentype tok;
};

sReservedWord reservedWords[] =
{
	//{"event", EVENT },
	{"if", IF },
	{"else", ELSE },
	{"while", WHILE },
	//{"fn", FUNCTION },
	//{"Event", EVENT},
	//{"UIEvent", UIEVENT},
	//{"CustomEvent", CUSTOMEVENT},
	//{"in",	ARG_IN},
	{"protocol", PROTOCOL},
	{"struct", STRUCT},
};
const int g_rlusize = sizeof(reservedWords) / sizeof(sReservedWord);

static eTokentype reservedLookup( char *s )
{
	int i=0;
	for( i=0; i < g_rlusize; ++i )
	{
		if( !_stricmp(s, reservedWords[ i].str) )
			return reservedWords[ i].tok;
	}
	return ID;
}


cProtocolScanner::cProtocolScanner() :
	m_fileMem(nullptr)
{
}

cProtocolScanner::~cProtocolScanner()
{
	Clear();
}


//----------------------------------------------------------------------------
// scann file
//----------------------------------------------------------------------------
bool cProtocolScanner::LoadFile( const string &fileName
	, bool isTrace //=false
)
{
	OFSTRUCT of;

	HFILE hFile = OpenFile(fileName.c_str(), &of, OF_READ);
	if (hFile != EOF)
	{
		const int fileSize = GetFileSize((HANDLE)hFile, nullptr);
		if (fileSize <= 0)
			return FALSE;

		SAFE_DELETEA(m_fileMem);

		DWORD readSize = 0;
		m_fileMem = new BYTE[ fileSize];
		ReadFile((HANDLE)hFile, m_fileMem, fileSize, &readSize, nullptr);
		CloseHandle((HANDLE)hFile);

		m_memSize = fileSize;
		m_isTrace = isTrace;
		Init();
		CheckUTF8WithBOM();
		return true;
	}

	std::cout << "[" << fileName << "] not found file\n";
	return false;
}


//----------------------------------------------------------------------------
// load from external memory
//----------------------------------------------------------------------------
bool cProtocolScanner::LoadPackageFile( BYTE *fileMem, int fileSize )
{
	SAFE_DELETEA(m_fileMem);

	m_memSize = fileSize;
	m_fileMem = new BYTE[ fileSize];
	memcpy(m_fileMem, fileMem, fileSize);

	Init();

	return true;
}


//----------------------------------------------------------------------------
// end of memory? or is load file?
//----------------------------------------------------------------------------
bool cProtocolScanner::IsEnd()
{
	if (!m_fileMem) return true;
	if (m_currentMemPoint >= m_memSize) return true;
	return false;
}


eTokentype cProtocolScanner::GetToken()
{
	if (!m_fileMem) return ENDFILE;

	if( m_tokQ.size() == 0 )
	{
		for (int i=0; i < MAX_QSIZE; ++i)
		{
			sTokDat d;
			char buf[ MAX_TOKENLEN];
			d.tok = _GetToken( buf );
			d.str = buf;
			m_tokQ.push_back( d );
		}
	}
	else
	{
		sTokDat d;
		char buf[ MAX_TOKENLEN];
		d.tok = _GetToken( buf );
		d.str = buf;
		m_tokQ.push_back( d );
		m_tokQ.pop_front();
	}

	return m_tokQ[ 0].tok;
}


eTokentype cProtocolScanner::GetTokenQ( int nIdx )
{
	if (!m_fileMem) return ENDFILE;

	return m_tokQ[ nIdx].tok;
}


char* cProtocolScanner::GetTokenStringQ( int nIdx )
{
	if (!m_fileMem) return nullptr;
	return 	(char*)m_tokQ[ nIdx].str.c_str();
}


char* cProtocolScanner::CopyTokenStringQ( int nIdx )
{
	if (!m_fileMem) return nullptr;

	int len = m_tokQ[ nIdx].str.size();
	char *p = new char[ len + 1];
	strcpy_s( p, len+1, m_tokQ[ nIdx].str.c_str() );
	return p;
}


char cProtocolScanner::GetNextChar()
{
	if (m_linePos >= m_bufSize)
	{
		++m_lineNo;
		if (GetString(m_buf, MAX_BUFF))
		{
			m_linePos = 0;
			m_bufSize = strlen(m_buf);
 			if (m_isTrace)
 				printf( "%4d: %s", m_lineNo, m_buf );
		}
		else 
		{
			return EOF;
		}
	}

	return m_buf[ m_linePos++];
}


//----------------------------------------------------------------------------
// read string from memory, delimeter: new line
//----------------------------------------------------------------------------
bool cProtocolScanner::GetString(char *receiveBuffer, int maxBufferLength)
{
	if (m_currentMemPoint >= m_memSize)
		return FALSE;

	int i=0;
	for (i=0; (i < maxBufferLength) && (m_currentMemPoint < m_memSize); ++i)
	{
		const int memPoint = m_currentMemPoint;

		if ('\r' == m_fileMem[ memPoint]) // new line?
		{
			m_currentMemPoint += 2;
			receiveBuffer[ i++] = '\n';
			break;
		}
		else
		{
			receiveBuffer[ i] = m_fileMem[ memPoint];
			++m_currentMemPoint;
		}
	}

	if (i < maxBufferLength)
		receiveBuffer[ i] = NULL;
	else
		receiveBuffer[ maxBufferLength-1] = NULL;

	return true;
}


void cProtocolScanner::UngetNextChar()
{
	--m_linePos;
}


// lexer
eTokentype cProtocolScanner::_GetToken( char *pToken )
{
	bool isFloat = false;
	eTokentype currentToken;
	eStateType state = START;

	int nTok = 0;
	while( DONE != state )
	{
		char c = GetNextChar();
		bool save = true;
		switch( state )
		{
		case START:
			if( isdigit(c) )
				state = INNUM;
			else if( isalpha(c) || '@' == c || '_' == c )
				state = INID;
			else if( '=' == c )
			{
				state = INEQ;
				save = false;
			}
			else if( '!' == c )
			{
				state = INNEQ;
				save = false;
			}
			else if( '|' == c )
			{
				state = INOR;
				save = false;
			}
			else if( '&' == c )
			{
				state = INAND;
				save = false;
			}
			else if( '"' == c )
			{
				state = INSTR;
				save = false;
			}
			else if( '/' == c )
			{
				state = INDIV;
				save = false;
			}
			else if( '$' == c )
			{
				state = INCOMMENT;
				save = false;
			}
			else if ( '-' == c)
			{
				state = INARROW;
				save = false;
			}
			else if ( '<' == c)
			{
				state = INLTEQ;
				save = false;
			}
			else if ( '>' == c)
			{
				state = INRTEQ;
				save = false;
			}
			else if (':' == c)
			{
				state = INSCOPE;
				save = false;
			}
			else if( (' ' == c) || ('\t' == c) || ('\n' == c) )
				save = false;
			else
			{
				state = DONE;
				switch( c )
				{
				case EOF:
					save = false;
					currentToken = ENDFILE;
					//					fclose( m_fp );
					//					m_fp = nullptr;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '%':
					currentToken = REMAINDER;
					break;

// 				case '<':
// 					currentToken = RT;
// 					break;
// 				case '>':
// 					currentToken = LT;
// 					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				case '[':
					currentToken = LBRACKET;
					break;
				case ']':
					currentToken = RBRACKET;
					break;
				case ',':
					currentToken = COMMA;
					break;
// 				case ':':
// 					currentToken = COLON;
// 					break;
				case ';':
					currentToken = SEMICOLON;
					break;
				default:
					currentToken = _ERROR;
					break;
				}
			}
			break;

		case INCOMMENT:
			save = false;
			if( '\n' == c ) state = START;
			break;

		case INMULTI_COMMENT:
			if('*' == c)
			{
				state = OUTMULTI_COMMENT;
			}
			break;

		case OUTMULTI_COMMENT:
			if ('/' == c)
			{
				state = START;
			}
			else
			{
				state = INMULTI_COMMENT;
			}
			break;

		case INNUM:
			if( !isdigit(c) && '.' != c )
			{
				UngetNextChar();
				save = false;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if( !isalpha(c) && !isdigit(c) && ('_' != c) && ('.' != c) && ('@' != c) )
			{
				UngetNextChar();
				save = false;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INDIV:
			if( '/' == c )
			{
				state = INCOMMENT;
			}
			else if ('*' == c)
			{
				state = INMULTI_COMMENT;
			}
			else
			{
				UngetNextChar();
				currentToken = DIV;
				state = DONE;
			}
			save = false;
			break;

		case INEQ:
			if( '=' == c )
			{
				currentToken = EQ;
			}
			else
			{
				UngetNextChar();
				currentToken = ASSIGN;
			}
			save = false;
			state = DONE;
			break;
		case INNEQ:
			if( '=' == c )
			{
				currentToken = NEQ;
			}
			else
			{
				currentToken = NEG;
 				UngetNextChar();
// 				currentToken = _ERROR;
			}
			save = false;
			state = DONE;
			break;
		case INOR:
			if( '|' == c )
			{
				currentToken = OR;
			}
			else
			{
				UngetNextChar();
				currentToken = _ERROR;
			}
			save = false;
			state = DONE;
			break;
		case INAND:
			if( '&' == c )
			{
				currentToken = AND;
			}
			else
			{
				currentToken = REF;
 				UngetNextChar();
// 				currentToken = _ERROR;
			}
			save = false;
			state = DONE;
			break;
		case INSTR:
			if( '"' == c )
			{
				save = false;
				state = DONE;
				currentToken = STRING;
			}
			break;

		case INARROW:
			if( '>' == c )
			{
				currentToken = ARROW;
			}
			else
			{
				UngetNextChar();
				currentToken = MINUS;
			}
			save = false;
			state = DONE;
			break;

		case INLTEQ:
			if( '=' == c )
			{
				currentToken = LTEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = LT;
			}
			save = false;
			state = DONE;
			break;

		case INRTEQ:
			if( '=' == c )
			{
				currentToken = RTEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = RT;
			}
			save = false;
			state = DONE;
			break;

		case INSCOPE:
			{
				if (':' == c)
				{
					currentToken = SCOPE;
				}
				else
				{
					UngetNextChar();
					currentToken = COLON;
				}
				save = false;
				state = DONE;
			}
			break;

		case DONE:
		default:
			printf( "scanner bug: state = %d", state );
			state = DONE;
			currentToken = _ERROR;
			break;
		}

		if( (save) && (nTok < MAX_TOKENLEN) )
		{
			//			if( (INNUM==state) && ('.' == c) )
			//				isFloat = true;

			pToken[ nTok++] = c;
		}
		else if (nTok >= MAX_TOKENLEN)
		{
			state = DONE;
			--nTok;
		}

		if( DONE == state )
		{
			pToken[ nTok] = '\0';
			if( ID == currentToken )
				currentToken = reservedLookup( pToken );
		}
	}

	if (m_isTrace)
	{
		printf( "    %d: ", m_lineNo);
		//printToken( currentToken, pString );
	}

	return currentToken;
}


// if UTF-8 with BOM, ignore BOM (byte order mark)
// https://stackoverflow.com/questions/6769311/how-windows-notepad-interpret-characters
void cProtocolScanner::CheckUTF8WithBOM()
{
	if (m_memSize < 3)
		return;

	if ((m_fileMem[0] == 0xEF)
		&& (m_fileMem[1] == 0xBB)
		&& (m_fileMem[2] == 0xBF))
	{
		m_currentMemPoint = 3;
	}
}


void cProtocolScanner::Init()
{
	m_lineNo = 0;
	m_linePos = 0;
	m_bufSize = 0;
	m_currentMemPoint = 0;
}


void cProtocolScanner::Clear()
{
	SAFE_DELETEA(m_fileMem);
}
