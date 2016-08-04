
#include "stdafx.h"
#include "graphscript.h"


using namespace cvproc;
using namespace cvproc::imagematch;

cGraphScript::cGraphScript()
{
}

cGraphScript::~cGraphScript()
{
	Clear();
}


bool cGraphScript::Read(const string &fileName)
{
	Clear();

	if (!m_parser.Read(fileName))
		return false;

	m_root = build(NULL, m_parser.m_treeRoot, NULL);

	return true;
}


void cGraphScript::buildAttributes(const sParseTree *node, const string &str, vector<string> &attributes)
{
	m_parser.m_lineStr = (char*)str.c_str();
	while (1)
	{
		const char *pid = m_parser.id();
		const char *pnum = (!*pid) ? m_parser.number() : NULL;

		if (*pid)
		{
			const string symb = m_parser.GetSymbol(pid);
			if (symb.empty())
			{
				attributes.push_back(pid);
			}
			else
			{
				char *old = m_parser.m_lineStr;
				buildAttributes(node, symb, attributes);
				m_parser.m_lineStr = old;
			}
		}
		else if (*pnum)
		{
			attributes.push_back(pnum);
		}
		else
		{
			dbg::ErrLog("line {%d} error!! tree attribute fail [%s]\n", node->lineNum, str.c_str());
			m_parser.m_isError = true;
			break;
		}

		// comma check
		m_parser.m_lineStr = m_parser.passBlank(m_parser.m_lineStr);
		if (!m_parser.m_lineStr)
			break;
		if (*m_parser.m_lineStr == ',')
			++m_parser.m_lineStr;
	}
}


// id,1 : 자식 노드에서 현재 노드로 올라올 수 없다. no upper traverse
void cGraphScript::setTreeAttribute(sParseTree *node, vector<string> &attribs)
{
	if (attribs.empty())
		return;

	strcpy_s(node->name, attribs[0].c_str());

	// check delay
	node->delay = 2.f; // default

	// 첫번째 속성값은 node id 이므로 무시된다.

	// delay_number
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("delay_");
		if (string::npos != pos)
		{
			// delay  값 설정
			const int valPos = attribs[i].find("_");
			const float delay = (float)atof(attribs[i].substr(valPos + 1).c_str());
			node->delay = delay;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// tag_id
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("tag_");
		if (string::npos != pos)
		{
			// tag 값 설정
			const int valPos = attribs[i].find("_");
			const string tag = attribs[i].substr(valPos + 1);
			strcpy_s(node->tag, tag.c_str());

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// noproc
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("noproc");
		if (string::npos != pos)
		{
			node->noProc = true;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// noup
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("noup");
		if (string::npos != pos)
		{
			node->noUpperTraverse = true;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// auto
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("auto");
		if (string::npos != pos)
		{
			node->isAuto = true;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// key_id
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("key");
		if (string::npos != pos)
		{
			// tag 값 설정
			const int valPos = attribs[i].find("_");
			const string keyboard = attribs[i].substr(valPos + 1);
			if (keyboard == "esc")
				node->key = VK_ESCAPE;
			else if (keyboard == "enter")
				node->key = VK_RETURN;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

	// sidemenu
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		const int pos = attribs[i].find("sidemenu");
		if (string::npos != pos)
		{
			node->isSideMenu = true;

			// remove attribute
			std::rotate(attribs.begin() + i, attribs.begin() + i + 1, attribs.end());
			attribs.pop_back();
			break;
		}
	}

}


cGraphScript::sNode* cGraphScript::build(sParseTree *parent, sParseTree *current, sNode *parentNode)
{
	RETV(!current, NULL);

	vector<string> attribs;
	buildAttributes(current, current->line, attribs);
	setTreeAttribute(current, attribs);

	sNode *linkNode = FindParent(parentNode, current->name);
	sNode *currentNode = NULL;

	// terminal node
	if (current->child == NULL)
	{
		if (linkNode)
		{
			parentNode->out.push_back(linkNode);
			// 링크로 넘어간 노드는 거꾸로 타고 갈수 없게 한다.
			// 그래서 in 에는 추가하지 않음.
		}
		else
		{
			currentNode = new sNode;
			currentNode->id = current->name;
			currentNode->tag = current->tag;
			currentNode->delay = current->delay;
			currentNode->noProc = current->noProc;
			currentNode->isAuto = current->isAuto;
			currentNode->isSideMenu = current->isSideMenu;
			currentNode->key = current->key;
			currentNode->noUpperTraverse = current->noUpperTraverse;
			m_nodes.push_back(currentNode);

			if (parentNode)
			{
				parentNode->out.push_back(currentNode);
				currentNode->in.push_back(parentNode);
			}

			build(parent, current->next, parentNode);
		}
	}
	else
	{ // none terminal node
		if (linkNode)
		{
			dbg::ErrLog("duplicate node id = [%s] \n", current->name);
			return NULL;
		}
		else
		{
			currentNode = new sNode;
			currentNode->id = current->name;
			currentNode->tag = current->tag;
			currentNode->delay = current->delay;
			currentNode->noProc = current->noProc;
			currentNode->isAuto = current->isAuto;
			currentNode->isSideMenu = current->isSideMenu;
			currentNode->key = current->key;
			currentNode->noUpperTraverse = current->noUpperTraverse;
			if (parentNode)
			{
				parentNode->out.push_back(currentNode);
				currentNode->in.push_back(parentNode);
			}

			m_nodes.push_back(currentNode);

			build(current, current->child, currentNode);
			build(parent, current->next, parentNode);
		}
	}

	return currentNode;
}


cGraphScript::sNode* cGraphScript::FindParent(sNode *current, const string &id)
{
	RETV(!current, NULL);
	RETV(id.empty(), NULL);

	CheckClearAllNode();
	return FindParentRec(current, id);
}

// 부모에서 id 노드가 있는지 검색한다. 찾으면 노드를 리턴
cGraphScript::sNode* cGraphScript::FindParentRec(sNode *current, const string &id)
{
	RETV(!current, NULL);

	if (current->id == id)
		return current;

	for each (auto node in current->in)
	{
		if (node->check) // already visit node
			continue;
		if (node->id == id)
			return node;
		node->check = true;
		if (sNode *p = FindParentRec(node, id))
			return p;
	}

	return NULL;
}


void cGraphScript::Clear()
{
	m_root = NULL;

	for each(auto p in m_nodes)
		delete p;
	m_nodes.clear();
	m_parser.Clear();
}


cGraphScript::sNode* cGraphScript::Find(const string &id)
{
	CheckClearAllNode();
	return TraverseRec(m_root, id);
}


cGraphScript::sNode* cGraphScript::Traverse(const string &id)
{
	CheckClearAllNode();
	return TraverseRec(m_root, id);
}


cGraphScript::sNode* cGraphScript::TraverseRec(sNode *current, const string &id)
{
	RETV(!current, NULL);

	current->check = true;

	if (current->id == id)
		return current;

	for each (auto node in current->in)
	{
		if (node->check) // already visit node
			continue;
		if (node->id == id)
			return node;
		node->check = true;
		if (sNode *p = TraverseRec(node, id))
			return p;
	}

	for each (auto node in current->out)
	{
		if (node->check) // already visit node
			continue;
		if (node->id == id)
			return node;
		node->check = true;
		if (sNode *p = TraverseRec(node, id))
			return p;
	}

	return NULL;
}


// from 노드에서 to 노드로 가는 루트를 저장해 리턴한다.
bool cGraphScript::FindRoute(const string &from, const string &to, OUT vector<sNode*> &out)
{
	RETV(from == to, true);

	sNode *start = Find(from);
	RETV(!start, false);

	CheckClearAllNode();

	return FindRouteRec(start, to, out);
}


bool cGraphScript::FindRoute(sNode*current, const string &to, OUT vector<sNode*> &out)
{
	CheckClearAllNode();
	return FindRouteRec(current, to, out);
}


// 
bool cGraphScript::FindRouteRec(sNode*current, const string &id, OUT vector<sNode*> &out)
{
	RETV(!current, false);

	current->check = true;
	out.push_back(current);

	if (current->id == id)
		return true;

	if (!current->noUpperTraverse)
	{
		for each (auto node in current->in)
		{
			if (node->check) // already visit node
				continue;
			if (node->isAuto) // 일정시간이 지나야 나오는 씬은, 경로찾기에서 제외시킨다.
				continue;

			if (node->id == id)
			{
				out.push_back(node);
				return true;
			}
			node->check = true;
			if (FindRouteRec(node, id, out))
				return true;
		}
	}

	for each (auto node in current->out)
	{
		if (node->check) // already visit node
			continue;
		if (node->isAuto) // 일정시간이 지나야 나오는 씬은, 경로찾기에서 제외시킨다.
			continue;

		if (node->id == id)
		{
			out.push_back(node);
			return true;
		}
		node->check = true;
		if (FindRouteRec(node, id, out))
			return true;
	}

	out.pop_back();

	return false;
}


// 그래프 자료구조를 순회하기 전에, 체크를 해제하고 순회한다.
// 중복 순회를 막기위해 쓰인다.
void cGraphScript::CheckClearAllNode()
{
	for each(auto p in m_nodes)
		p->check = false;
}