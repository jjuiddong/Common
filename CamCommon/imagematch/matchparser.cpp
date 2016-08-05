
#include "stdafx.h"
#include "matchparser.h"


namespace cvproc {
	namespace imagematch {

		vector< vector<string>* > g_stringTable;


		// imagematch.h 에 선언됨.
		void ReleaseImageMatch()
		{
			for each (auto p in g_stringTable)
				delete p;
			g_stringTable.clear();
		}

	}
}


using namespace cvproc;
using namespace cvproc::imagematch;
using namespace std;


cParser2::cParser2()
	: m_treeRoot(NULL)
	, m_execRoot(NULL)
{
	m_symbolTree.reserve(4);
}

cParser2::~cParser2()
{
	Clear();
}


bool cParser2::IsBlank(const char *str, const int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (!*str)
			break;
		if (('\n' == *str) || ('\r' == *str) || ('\t' == *str) || (' ' == *str))
			++str;
		else
			return false;
	}
	return true;
}


// pass through blank character
char* cParser2::passBlank(const char *str)
{
	RETV(!str, NULL);

	while (1)
	{
		if (!*str)
			return NULL;
		if ((*str == ' ') || (*str == '\t'))
			++str;
		else
			break;
	}
	return (char*)str;
}

// id -> alphabet + {alphabet | number}
const char* cParser2::id()
{
	// comma check
	m_lineStr = passBlank(m_lineStr);
	if (!m_lineStr)
		return "";

	// 쌍따옴표로 id를 설정할 때
	if (*m_lineStr == '\"')
	{
		++m_lineStr;

		int cnt = 0;
		char *dst = m_tmpBuffer;
		while ((*m_lineStr != '\"') && (cnt < 256) && (*m_lineStr))
		{
			*dst++ = *m_lineStr++;
			++cnt;
		}
		*dst = NULL;
		++m_lineStr;
		return m_tmpBuffer;
	}



	// find first char
	const char *n = strchr(g_strStr, *m_lineStr);
	if (!n)
		return "";

	char *dst = m_tmpBuffer;
	*dst++ = *m_lineStr++;

	while (1)
	{
		const char *n = strchr(g_strStr2, *m_lineStr);
		if (!n || !*n)
			break;
		*dst++ = *m_lineStr++;
	}

	*dst = NULL;
	return m_tmpBuffer;
}

const char* cParser2::number()
{
	// comma check
	m_lineStr = passBlank(m_lineStr);
	if (!m_lineStr)
		return "";

	char *dst = m_tmpBuffer;
	if ('+' == *m_lineStr)
		*dst++ = *m_lineStr++;
	else if ('-' == *m_lineStr)
		*dst++ = *m_lineStr++;

	while (1)
	{
		const char *n = strchr(g_numStr, *m_lineStr);
		if (!n || !*n)
			break;
		*dst++ = *m_lineStr++;
	}

	*dst = NULL;
	return m_tmpBuffer;
}

char cParser2::op()
{
	m_lineStr = passBlank(m_lineStr);
	if (!m_lineStr)
		return NULL;
	const char *n = strchr(g_strOp, *m_lineStr);
	if (!n)
		return NULL;
	return *m_lineStr++;
}

char cParser2::comma()
{
	m_lineStr = passBlank(m_lineStr);
	if (!m_lineStr)
		return NULL;
	const char *n = strchr(",", *m_lineStr);
	if (!n)
		return NULL;
	return *m_lineStr++;
}

int cParser2::assigned(const char *var)
{
	m_lineStr = passBlank(m_lineStr);
	if (!m_lineStr)
	{
		dbg::ErrLog("line {%d} error!! assigned \n", m_lineNum);
		return 0;
	}

	// array type
	if ('[' == m_lineStr[0]) 
	{
		++m_lineStr;

		string symb = var;

		// symbol = '[' string list ']'
		vector<string> table;
		while (1)
		{
			const string str = id();
			if (str.empty())
				break;
			table.push_back(str);
			if (',' != comma())
				break;
		}

		SetSymbol(symb, table);

		m_lineStr = passBlank(m_lineStr);
		if (*m_lineStr != ']')
		{ // error 
			dbg::ErrLog("line {%d} assigned error!! not found ']' %s \n", m_lineNum, m_lineStr);
			return 0;
		}
		++m_lineStr;
	}
	else
	{
		SetSymbol(var, m_lineStr);
	}

	return 1;
}


// 할당 연산자가 없으면 false를 리턴한다.
// { id = id {, id = id } }
//  적어도 심볼이 1개 이상 추가했을 때만, true 를 리턴한다.
bool cParser2::assigned_list(const sParseTree *node)
{
	m_lineStr = passBlank(m_lineStr);
	RETV(!m_lineStr, false);

	if (*m_lineStr != '{')
		return false;

	++m_lineStr;

	AddSymbolTable();

	int addCount = 0;
	while (1)
	{
		const string symb = id();
		if (symb.empty())
		{
			dbg::ErrLog("line {%d} assigned_list error!! not found id %s \n", node->lineNum, node->line);
			break;
		}

		const char c = op();

		if (c != '=')
		{
			dbg::ErrLog("line {%d} assigned_list error!! not found '=' %s \n", node->lineNum, node->line);
			break;
		}

		const string data = id();
		if (data.empty())
		{
			dbg::ErrLog("line {%d} assigned_list error!! not found data %s \n", node->lineNum, node->line);
			break;
		}

		++addCount;
		SetSymbol(symb, data);

		if (',' != comma())
			break;

		++m_lineStr;
	}

	m_lineStr = passBlank(m_lineStr);
	if (*m_lineStr != '}')
	{ // error 
		dbg::ErrLog("line {%d} assigned_list error!! not found '}' %s \n", node->lineNum, node->line);
		RemoveSymbolTable();
		return false;
	}

	++m_lineStr;

	if (addCount <= 0)
		RemoveSymbolTable();

	return (addCount>0); //  적어도 1개 이상 추가했을 때만, true 를 리턴한다.
}


// 한 라인을 읽는다.
// 첫 번째 문자가 # 이면 주석 처리된다. 
// 문자열 중간에 # 이 있을 때는 처리 하지 않는다.
bool cParser2::ScanLine()
{
	if (m_isReverseScan)
	{
		--m_lineNum;
		m_lineStr = m_scanLine;
		m_isReverseScan = false;
		return true;
	}

	do
	{
		++m_lineNum;
		if (!m_ifs.getline(m_scanLine, sizeof(m_scanLine)))
			return false;
	} while (IsBlank(m_scanLine, sizeof(m_scanLine)) || (m_scanLine[0] == '#'));

	m_lineStr = m_scanLine;
	return true;
}


// 읽는 위치를 한 라인위로 옮긴다.
void cParser2::UnScanLine()
{
	m_isReverseScan = true;
}


void cParser2::collectTree(const sParseTree *current, set<sParseTree*> &out) const
{
	if (!current)
		return;
	out.insert((sParseTree *)current);
	collectTree(current->child, out);
	collectTree(current->next, out);
}


void cParser2::removeTree(const sParseTree *current) const
{
	set<sParseTree*> rmNodes;// 중복된 노드를 제거하지 않기위해 사용됨
	collectTree(current, rmNodes);
	for each (auto &node in rmNodes)
		delete node;
}


sParseTree* cParser2::cloneTree(const sParseTree *current) const
{
	RETV(!current, NULL);

	sParseTree*node = new sParseTree;
	memcpy(node, current, sizeof(sParseTree));
	node->child = cloneTree(current->child);
	node->next = cloneTree(current->next);
	return node;
}


sParseTree* cParser2::tree(sParseTree *current, const int depth)
{
	if (!ScanLine())
		return NULL;

	int cntDepth = 0;
	while (1)
	{
		if (!*m_lineStr)
			break; // error

		if (*m_lineStr == '\t')
		{
			++cntDepth;
			++m_lineStr;
		}
		else
		{
			break; // end
		}
	}

	if (cntDepth <= 0)
		return NULL; // error

	sParseTree *node = new sParseTree;
	memset(node, 0, sizeof(sParseTree));
	node->lineNum = m_lineNum;
	node->depth = cntDepth;
	strcpy_s(node->line, m_lineStr);

	sParseTree *reval = NULL;
	if (cntDepth == depth)
	{
		current->next = node;
		reval = tree(node, cntDepth);
	}
	else if (cntDepth == (depth + 1))
	{
		current->child = node;
		reval = tree(node, cntDepth);
	}
	else if (cntDepth > depth + 1)
	{
		// error
		dbg::ErrLog("line {%d} tree depth error!! %s \n", m_lineNum, node->line);
		return NULL;
	}
	else
	{
		return node;
	}

	// search parent tree node
	while (reval)
	{
		if (depth == reval->depth)
		{
			current->next = reval;
			reval = tree(reval, depth);
		}
		else
		{
			return reval;
		}
	}

	return NULL;
}


// 스크립트를 읽고, 파스트리를 생성한다.
bool cParser2::Read(const string &fileName)
{
	Clear();

	m_ifs.close();
	m_ifs.open(fileName.c_str());
	if (!m_ifs.is_open())
		return false;

	AddSymbolTable(); // init root symbol tabe

	sParseTree *currentTree = NULL;
	sParseTree *currentExecTree = NULL;
	while (ScanLine())
	{
		const char *str = id();
		if (!*str)
			continue;

		const char c = op();

		if (c == ':') // tree head
		{
			sParseTree *head = new sParseTree;
			memset(head, 0, sizeof(sParseTree));
			head->type = 0;
			strcpy_s(head->line, str);

			if (!m_treeRoot)
			{
				m_treeRoot = head;
				currentTree = head;
			}
			else
			{
				currentTree->next = head;
				currentTree = head;
			}

 			tree(head, 0); // build tree
			UnScanLine();
		}
		else if (c == '=')
		{
 			if (!assigned(str))
 				break;
		}
		else
		{
			// exec command
			if (string(str) == "exec")
			{
				sParseTree *node = new sParseTree;
				memset(node, 0, sizeof(sParseTree));
				node->type = 1;
				strcpy_s(node->line, m_lineStr);

				if (!m_execRoot)
				{
					m_execRoot = node;
					currentExecTree = node;
				}
				else
				{
					currentExecTree->next = node;
					currentExecTree = node;
				}
			}
			else if (string(str) == "include") // include filename
			{
				const string includeFileName = m_lineStr;
				cParser2 parser;
				if (parser.Read(includeFileName))
				{
					sParseTree *node = parser.cloneTree(parser.m_treeRoot);

					if (!m_treeRoot)
						m_treeRoot = node;
					else
						currentTree->next = node;

					while (node->next)
						node = node->next;
					currentTree = node;
				}
			}
			else
			{
				// error occur
				dbg::ErrLog("line {%d} error!! operator >> ", m_lineNum);
				if (m_lineStr)
					dbg::ErrLog("code = %s", m_lineStr);
				dbg::ErrLog(" [%c]\n", c);
				m_isError = true;
				break;
			}
		}
	}

	m_ifs.close();

	if (m_isError)
		return false;

	return true;
}


void cParser2::Clear()
{
	removeTree(m_treeRoot);
	removeTree(m_execRoot);
	m_treeRoot = NULL;
	m_execRoot = NULL;
	m_isReverseScan = false;
	m_isError = false;
	m_lineNum = 0;

	m_symbolTree.clear();
}


// 심볼 데이타 리턴
string cParser2::GetSymbol(const string &symbol)
{
	for (int i = m_symbolTree.size()-1; i >= 0; --i)
	{
		auto it = m_symbolTree[i].find(symbol);
		if (m_symbolTree[i].end() == it)
			continue;
		return it->second.str;
	}

	return "";
}

vector<string>* cParser2::GetSymbol2(const string &symbol)
{
	for (int i = m_symbolTree.size() - 1; i >= 0; --i)
	{
		auto it = m_symbolTree[i].find(symbol);
		if (m_symbolTree[i].end() == it)
			continue;
		return it->second.table;
	}

	return NULL;
}


// 심볼이 타입을 리턴한다.
int cParser2::GetSymbolType(const string &symbol)
{
	for (int i = m_symbolTree.size() - 1; i >= 0; --i)
	{
		auto it = m_symbolTree[i].find(symbol);
		if (m_symbolTree[i].end() == it)
			continue;
		return it->second.type;
	}
	return 0;
}


bool cParser2::SetSymbol(const string &key, const string data)
{
	RETV(m_symbolTree.empty(), false);

	m_symbolTree.back()[key].type = 0;
	m_symbolTree.back()[key].str = data;
	return true;
}


bool cParser2::SetSymbol(const string &key, const vector<string> &table)
{
	RETV(m_symbolTree.empty(), false);

	// 전역 스트링 테이블에 저장된다.
	vector<string> *p = new vector<string>();
	*p = table;
	g_stringTable.push_back(p);
	//

	m_symbolTree.back()[key].type = 1;
	m_symbolTree.back()[key].table = p;
	return true;
}


// 심볼 테이블을 하위에 추가한다.
bool cParser2::AddSymbolTable()
{
	m_symbolTree.push_back(map<string, sSymbolData>());
	return true;
}


// 가장 최근에 추가한 심볼테이블을 제거한다.
bool cParser2::RemoveSymbolTable()
{
	m_symbolTree.pop_back();
	return true;
}
