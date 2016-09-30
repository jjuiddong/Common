
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


cGraphScript::sNode* cGraphScript::build(sParseTree *parent, sParseTree *current, sNode *parentNode)
{
	RETV(!current, NULL);

	if (current->attrs["delay"].empty())
		current->attrs["delay"] = "0.5"; // default

	sNode *linkNode = FindParent(parentNode, current->attrs["id"]);
	sNode *headNode = FindHead(current->attrs["id"]);
	sNode *srcNode = (headNode) ? headNode : linkNode;

	if (current->child && srcNode && !headNode)
	{
		dbg::ErrLog("duplicate node id = [%s] \n", current->attrs["id"].c_str());
		return NULL;
	}

	sNode *newNode = srcNode;

	// parent setting node
	if (string("parent") == current->attrs["id"])
	{
		if (parentNode)
		{
			// node name을 제외한 모든 속성들을 설정한다.
			const string tmpId = parentNode->attrs["id"];
			parentNode->attrs = current->attrs;
			parentNode->attrs["id"] = tmpId;
			parentNode->sceneId = (current->attrs["sceneid"].empty())? m_sceneIdGen++ : atoi(current->attrs["sceneid"].c_str());
		}
	} 
	else if (!newNode)
	{
		newNode = new sNode;
		newNode->attrs = current->attrs;
		newNode->sceneId = atoi(current->attrs["sceneid"].c_str());

		if (parentNode)
		{
 			if (current->attrs["sceneid_inherit"] == "1") 
				newNode->sceneId = parentNode->sceneId ;
			if (parentNode->attrs["sceneid_child_inherit"] == "1")
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
		if (newNode && (newNode->attrs["sidesel"] == "1"))
		{
			newNode->attrs["sidesel"] = "0";
			for each (auto next in newNode->out)
				next->attrs["sidesel"] = "1";
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

	if (current->attrs["id"] == id)
		return current;

	for each (auto node in current->in)
	{
		if (node->check) // already visit node
			continue;
		if (node->attrs["id"] == id)
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
	for each (auto &node in m_nodes)
	{
		if (node->attrs["id"] == id)
			return node;
	}
	return NULL;
}


// current를 중심으로 동일한 ID를 가진 노드를 리턴한다.
cGraphScript::sNode* cGraphScript::Find(sNode*current, const string &id)
{
	if (!current)
		return Find(id);

	vector<sNode*> path;
	FindRoute(current, id, path);
	if (path.empty())
		return NULL;

	return path.back();
}


cGraphScript::sNode* cGraphScript::FindHead(const string &id)
{
	for each (auto &node in m_heads)
	{
		if (node->attrs["id"] == id)
			return node;
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


// 가장 짧은 경로를 리턴한다.
bool cGraphScript::FindRouteRec(sNode*current, const string &id, OUT vector<sNode*> &out)
{
	RETV(!current, false);

	current->check = true;

	vector<sNode*> path1, path2;
	path1.push_back(current);
	path2.push_back(current);

	if (current->attrs["id"] == id)
		return true;

	if (current->attrs["noup"] != "1")
	{
		for each (auto node in current->in)
		{
			if (node->check) // already visit node
				continue;
			if (node->attrs["auto"] == "1") // 일정시간이 지나야 나오는 씬은, 경로찾기에서 제외시킨다.
				continue;

			if (node->attrs["id"] == id)
			{
				path1.push_back(node);
				break;
			}
			node->check = true;
			if (FindRouteRec(node, id, path1))
				break;
		}
	}

	for each (auto node in current->out)
	{
		if (node->check) // already visit node
			continue;
		if (node->attrs["auto"] == "1") // 일정시간이 지나야 나오는 씬은, 경로찾기에서 제외시킨다.
			continue;

		if (node->attrs["id"] == id)
		{
			path2.push_back(node);
			break;
		}
		node->check = true;
		if (FindRouteRec(node, id, path2))
			break;
	}


	if (!path1.empty() && !path2.empty())
	{
		// 두 경로 모두 목적지에 도착 했다면, 
		if ((path1.back()->attrs["id"] == id) && (path2.back()->attrs["id"] == id))
		{
			// 더 짧은 경로를 저장한다.
			if (path1.size() <= path2.size())
				std::copy(path1.begin(), path1.end(), std::back_inserter(out));
			else
				std::copy(path2.begin(), path2.end(), std::back_inserter(out));
		}
		else if (path1.back()->attrs["id"] == id) // 경로 1만 목적지에 도달했다면
		{
			std::copy(path1.begin(), path1.end(), std::back_inserter(out));
		}
		else if(path2.back()->attrs["id"] == id) // 경로 2만 목적지에 도달했다면
		{
			std::copy(path2.begin(), path2.end(), std::back_inserter(out));
		}
	}
	else if (!path1.empty())
	{
		if (path1.back()->attrs["id"] == id) // 목적지에 도착했다면.
			std::copy(path1.begin(), path1.end(), std::back_inserter(out));
	}
	else if (!path2.empty())
	{
		if (path2.back()->attrs["id"] == id) // 목적지에 도착했다면.
			std::copy(path2.begin(), path2.end(), std::back_inserter(out));
	}

	// 목적지에 도착했다면 true를 리턴한다.
	if (!out.empty() && (out.back()->attrs["id"] == id))
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


void PrintGraphSub(std::ofstream &ofs, cGraphScript::sNode *current, const int tab)
{
	using namespace std;
	RET(!current);

	for (int i = 0; i < tab; ++i)
		ofs << "\t";
	ofs << current->attrs["id"] << endl;

	RET(current->check);
	
	current->check = true;
	for each (auto &out in current->out)
		PrintGraphSub(ofs, out, tab + 1);
}


void cGraphScript::PrintGraph(const string &rootName)
{
	sNode *root = FindHead(rootName);
	RET(!root);

	CheckClearAllNode();

	std::ofstream ofs("print_graph.txt");
	if (ofs.is_open())
		PrintGraphSub(ofs, root, 0);
}
