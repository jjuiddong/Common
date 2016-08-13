
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
	node->delay = 0.5f; // default

	// 첫번째 속성값은 node id 이므로 무시된다.
	for (int i = 1; i < (int)attribs.size(); ++i)
	{
		if (string::npos != attribs[i].find("delay_"))
		{
			// delay_num, delay  값 설정
			const int valPos = attribs[i].find("_");
			const float delay = (float)atof(attribs[i].substr(valPos + 1).c_str());
			node->delay = delay;
		}
		else if (string::npos != attribs[i].find("tag_"))
		{
			// tag_id, tag 값 설정
			const int valPos = attribs[i].find("_");
			const string tag = attribs[i].substr(valPos + 1);
			strcpy_s(node->tag, tag.c_str());
		}
		else if (string::npos != attribs[i].find("noproc"))
		{
			// noproc
			node->noProc = true;
		}
		else if (string::npos != attribs[i].find("noup"))
		{
			// noup
			node->noUpperTraverse = true;
		}
		else if (string::npos != attribs[i].find("auto"))
		{
			// auto
			node->isAuto = true;
		}
		else if (string::npos != attribs[i].find("key"))
		{
			// key_id 값 설정
			const int valPos = attribs[i].find("_");
			const string keyboard = attribs[i].substr(valPos + 1);
			if (keyboard == "esc")
				node->key = VK_ESCAPE;
			else if (keyboard == "enter")
				node->key = VK_RETURN;
		}
		else if (string::npos != attribs[i].find("sidemenu"))
		{
			// sidemenu
			node->isSideMenu = true;
		}
		else if (string::npos != attribs[i].find("enterchild"))
		{
			// enterchild
			node->isEnterChild = true;
		}
		else if (string::npos != attribs[i].find("nomenu"))
		{
			// nomenu
			node->isNoMenu = true;
		}
		else if (string::npos != attribs[i].find("sidesel"))
		{
			// sidesel
			node->isSideSubmenu = true;
		}
		else if (string::npos != attribs[i].find("circular"))
		{
			// circular
			node->isCircularMenu = true;
		}
		else if (string::npos != attribs[i].find("sceneid_"))
		{
			// sceneid_~~
			// sceneid_inherit 부모 노드로 부터 상속받는다. 동일한 ID 를 가짐.
			// sceneid_child_inherit, 현재 노드의 Scene ID를 자식 노드에게 상속시킨다.
			if (attribs[i] == "sceneid_inherit")
			{
				node->isSceneIdInherit = true;
			}
			else if (attribs[i] == "sceneid_child_inherit")
			{
				node->isSceneIdChildInherit = true;
			}
			else
			{
				const int valPos = attribs[i].find("_");
				const int id = atoi(attribs[i].substr(valPos + 1).c_str());
				node->sceneId = id;
			}
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
	sNode *headNode = FindHead(current->name);
	sNode *srcNode = (headNode) ? headNode : linkNode;

	// terminal node
	if (current->child == NULL)
	{
		if (srcNode)
		{
			parentNode->out.push_back(srcNode);
			// 링크로 넘어간 노드는 거꾸로 타고 갈수 없게 한다.
			// 그래서 in 에는 추가하지 않음.
		}
	}
	else
	{ // none terminal node
		if (srcNode && !headNode)
		{
			dbg::ErrLog("duplicate node id = [%s] \n", current->name);
			return NULL;
		}
	}

	sNode *newNode = srcNode;

	// parent setting node
	if (string("parent") == current->name)
	{
		if (parentNode)
		{
			// node name을 제외한 모든 속성들을 설정한다.
			parentNode->tag = current->tag;
			parentNode->delay = current->delay;
			parentNode->noProc = current->noProc;
			parentNode->isAuto = current->isAuto;
			parentNode->isSideMenu = current->isSideMenu;
			parentNode->key = current->key;
			parentNode->noUpperTraverse = current->noUpperTraverse;
			parentNode->isEnterChild = current->isEnterChild;
			parentNode->isNoMenu = current->isNoMenu;
			parentNode->isSideSubmenu = current->isSideSubmenu;
			parentNode->isSceneIdInherit = current->isSceneIdInherit;
			parentNode->isSceneIdChildInherit = current->isSceneIdChildInherit;
			parentNode->isCircularMenu = current->isCircularMenu;

			parentNode->sceneId = (current->sceneId == 0) ? m_sceneIdGen++ : current->sceneId;
		}
	} 
	else if (!newNode)
	{
		newNode = new sNode;
		newNode->id = current->name;
		newNode->tag = current->tag;
		newNode->delay = current->delay;
		newNode->noProc = current->noProc;
		newNode->isAuto = current->isAuto;
		newNode->isSideMenu = current->isSideMenu;
		newNode->key = current->key;
		newNode->noUpperTraverse = current->noUpperTraverse;
		newNode->isEnterChild = current->isEnterChild;
		newNode->isNoMenu = current->isNoMenu;
		newNode->isSideSubmenu = current->isSideSubmenu;
		newNode->isSceneIdInherit = current->isSceneIdInherit;
		newNode->isSceneIdChildInherit = current->isSceneIdChildInherit;
		newNode->isCircularMenu = current->isCircularMenu;
		newNode->sceneId = current->sceneId;

		if (parentNode)
		{
 			if (current->isSceneIdInherit) 
				newNode->sceneId = parentNode->sceneId ;
			if (parentNode->isSceneIdChildInherit)
				newNode->sceneId = parentNode->sceneId;
		}

		if (newNode->sceneId == 0)
			newNode->sceneId = m_sceneIdGen++;

		m_nodes.push_back(newNode);
	}

	if (srcNode)
	{
		if (parentNode)
		{
			parentNode->out.push_back(srcNode);
			srcNode->in.push_back(parentNode);
		}

		build(current, current->child, srcNode);
		build(parent, current->next, parentNode);
	}
	else if (newNode)
	{
		// Link 노드가 아닐때, 노드간 연결 (Link 노드는 이미 연결되어 있음)
		if (parentNode)
		{
			parentNode->out.push_back(newNode);
			newNode->in.push_back(parentNode);
		}
		else
		{
			m_heads.push_back(newNode); // 헤드 노드
		}

		build(current, current->child, newNode);

		// 부모가 sidesel 속성을 가질 경우, child 도 모두 side sel 속성을 가지게 한다.
		// 그리고 부모의 sidesel 속성은 제거된다.
		if (newNode && newNode->isSideSubmenu)
		{
			newNode->isSideSubmenu = false;
			for each (auto next in newNode->out)
				next->isSideSubmenu = true;
		}

		build(parent, current->next, parentNode);
	}
	else
	{
		// parent node
		build(parent, current->next, parentNode);
	}

	return newNode;
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
	m_sceneIdGen = 1;

	for each(auto p in m_nodes)
		delete p;
	m_nodes.clear();
	m_heads.clear();
	m_parser.Clear();
}


cGraphScript::sNode* cGraphScript::Find(const string &id)
{
//	CheckClearAllNode();
//	return TraverseRec(m_root, id);
	for each (auto &node in m_nodes)
	{
		if (node->id == id)
			return node;
	}
	return NULL;
}


cGraphScript::sNode* cGraphScript::FindHead(const string &id)
{
	for each (auto &node in m_heads)
	{
		if (node->id == id)
			return node;
	}
	return NULL;
}

// 
// cGraphScript::sNode* cGraphScript::Traverse(const string &id)
// {
// 	CheckClearAllNode();
// 	return TraverseRec(m_root, id);
// }
// 
// 
// cGraphScript::sNode* cGraphScript::TraverseRec(sNode *current, const string &id)
// {
// 	RETV(!current, NULL);
// 
// 	current->check = true;
// 
// 	if (current->id == id)
// 		return current;
// 
// 	for each (auto node in current->in)
// 	{
// 		if (node->check) // already visit node
// 			continue;
// 		if (node->id == id)
// 			return node;
// 		node->check = true;
// 		if (sNode *p = TraverseRec(node, id))
// 			return p;
// 	}
// 
// 	for each (auto node in current->out)
// 	{
// 		if (node->check) // already visit node
// 			continue;
// 		if (node->id == id)
// 			return node;
// 		node->check = true;
// 		if (sNode *p = TraverseRec(node, id))
// 			return p;
// 	}
// 
// 	return NULL;
// }


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


// 가장 짧은 경로를 리턴한다.
bool cGraphScript::FindRouteRec(sNode*current, const string &id, OUT vector<sNode*> &out)
{
	RETV(!current, false);

	current->check = true;

	vector<sNode*> path1, path2;
	path1.push_back(current);
	path2.push_back(current);
	//out.push_back(current);

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
				path1.push_back(node);
				break;// return true;
			}
			node->check = true;
			if (FindRouteRec(node, id, path1))
				break;//return true;
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
			path2.push_back(node);
			break;// return true;
		}
		node->check = true;
		if (FindRouteRec(node, id, path2))
			break;// return true;
	}


	if (!path1.empty() && !path2.empty())
	{
		// 두 경로 모두 목적지에 도착 했다면, 
		if ((path1.back()->id == id) && (path2.back()->id == id))
		{
			// 더 짧은 경로를 저장한다.
			if (path1.size() <= path2.size())
				std::copy(path1.begin(), path1.end(), std::back_inserter(out));
			else
				std::copy(path2.begin(), path2.end(), std::back_inserter(out));
		}
		else if (path1.back()->id == id) // 경로 1만 목적지에 도달했다면
		{
			std::copy(path1.begin(), path1.end(), std::back_inserter(out));
		}
		else if(path2.back()->id == id) // 경로 2만 목적지에 도달했다면
		{
			std::copy(path2.begin(), path2.end(), std::back_inserter(out));
		}
	}
	else if (!path1.empty())
	{
		if (path1.back()->id == id) // 목적지에 도착했다면.
			std::copy(path1.begin(), path1.end(), std::back_inserter(out));
	}
	else if (!path2.empty())
	{
		if (path2.back()->id == id) // 목적지에 도착했다면.
			std::copy(path2.begin(), path2.end(), std::back_inserter(out));
	}

	// 목적지에 도착했다면 true를 리턴한다.
	if (!out.empty() && (out.back()->id == id))
		return true;

	return false;
}


// 그래프 자료구조를 순회하기 전에, 체크를 해제하고 순회한다.
// 중복 순회를 막기위해 쓰인다.
void cGraphScript::CheckClearAllNode()
{
	for each(auto p in m_nodes)
		p->check = false;
}