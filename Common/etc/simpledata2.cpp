
#include "stdafx.h"
#include "simpledata2.h"

using namespace common;


cSimpleData2::cSimpleData2(const vector<sRule> &rules //= {}
)
	: m_rules(rules)
	, m_root(nullptr)
{
}

cSimpleData2::~cSimpleData2()
{
	Clear();
}


bool cSimpleData2::Read(const StrPath &fileName)
{
	Clear();

	using namespace std;
	ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		return false;

	map<int, sNode*> parents; // state, parent node
	m_root = new sNode;
	m_root->name = "root";
	parents[0] = m_root;
	sNode *node = m_root;

	int state = 0;
	string line;
	while (getline(ifs, line))
	{
		common::trim(line);
		vector<string> toks;
		common::tokenizer_space(line, toks);
		if (toks.empty())
			continue;

		// check comment, two slash //
		if (toks[0].size() >= 2)
		{
			if (('/' == toks[0][0]) && ('/' == toks[0][1]))
				continue; // ignore this line
		}

		sRule *rule = FindRule(state, toks[0]);
		if (rule)
		{
			if ((rule->nstate == state) && (rule->parent < 0))
			{
				node->attrs[toks[0]] = toks;
			}
			else // change state
			{
				sNode *p = new sNode;
				p->name = toks[0];
				if (rule->parent < 0)
					parents[state]->children.push_back(p); // down
				else
					parents[rule->parent]->children.push_back(p); // up

				parents[rule->nstate] = p;
				node = p;
				state = rule->nstate;
			}
		}
		else
		{
			node->attrs[toks[0]] = toks;
		}
	}

	return true;
}


// find next rule
cSimpleData2::sRule* cSimpleData2::FindRule(const int curState, const string &str)
{
	sRule *r = nullptr;
	for (auto &rule : m_rules)
	{
		if (rule.pstate != curState)
			continue;

		if ((rule.pstate == curState)
			&& (rule.tok == str))
		{
			assert(!r); // check duplicate
			r = &rule;
		}
	}
	return r;
}


// return key : value
// return array, if array size more than 2
vector<string>& cSimpleData2::GetArray(sNode *node, const string &key)
{
	static vector<string> emptyVal;

	auto it = node->attrs.find(key);
	if (node->attrs.end() == it)
		return emptyVal;
	if (it->second.size() <= 1)
		return emptyVal;
	return it->second;
}


void cSimpleData2::Clear()
{
	RemoveNode(m_root);
	m_root = nullptr;
}


void cSimpleData2::RemoveNode(sNode *node)
{
	RET(!node);

	for (auto &c : node->children)
		RemoveNode(c);
	node->children.clear();
	delete node;
}
