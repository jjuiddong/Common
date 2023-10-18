//
// 2019-11-19, jjuiddong
// simple data read/write class
//
// sample data 1
// str0 (state change: state0 -> state1)
//		str1 str2 (state1)
//		str3 str4 (state1)
//		str5 (state change: state1 -> state2)
//			str1 str2 (state2)
//			str3 str4 (state2)
//		str6 (state change: state1 -> state3)
//			str1 str2 (state3)
//			str3 str4 (state3)
//
// rule sample
//	vector<common::cSimpleData2::sRule> rules;
//	rules.push_back({ 0, "str0", 1, -1 });
//	rules.push_back({ 1, "str5", 2, -1 });
//	rules.push_back({ 2, "str5", 2, 1 });
//	rules.push_back({ 3, "str5", 2, 1 });
//	rules.push_back({ 1, "str6", 3, -1 });
//	rules.push_back({ 3, "str6", 3, 1 });
//	rules.push_back({ 2, "str6", 3, 1 });
//	rules.push_back({ 2, "str0", 1, 0 });
//	rules.push_back({ 3, "str0", 1, 0 });
// -----------------------------------------------------------------------
// sample data 2
//link
//	name base
//	geometry box
//	scale 1 1 1
//link
//	name arm1
//	geometry box
//	scale 1 1 1
//link
//	name arm2
//	geometry box
//	scale 1 1 1
//joint
//	name joint1
//	type revolute
//	parent base
//	child arm1
//	origin_rpy 0 0 0
//	origin_xyz 0 0 0
//	axis_xyz 0 1 0
// joint
//	name joint2
//	type revolute
//	parent arm1
//	child arm2
//	origin_rpy 0 0 0
//	origin_xyz 0 0 0
//	axis_xyz 0 1 0
//
//rule
// vector<cSimpleData2::sRule> rules;
// rules.push_back({ 0, "link", 1, 0 });
// rules.push_back({ 1, "link", 1, 0 });
// rules.push_back({ 2, "link", 1, 0 });
// rules.push_back({ 0, "joint", 2, 0 });
// rules.push_back({ 1, "joint", 2, 0 });
// rules.push_back({ 2, "joint", 2, 0 });
// -----------------------------------------------------------------------

#pragma once


namespace common
{

	class cSimpleData2
	{
	public:
		// state change rule
		struct sRule
		{
			int pstate; // prev state
			string tok; // change state token
			int nstate; // next state
			int parent; // parent node state
		};
		struct sNode
		{
			string name;
			map<string, vector<string>> attrs; //key, values
			vector<sNode*> children;
		};
		cSimpleData2(const vector<sRule> &rules = {});
		virtual ~cSimpleData2();

		bool Read(const StrPath &fileName);
		bool Read(std::istream &is);
		vector<string>& GetArray(sNode *node, const string &key);
		template<class T> const T Get(sNode *node, const string &key, const T &defaultValue);
		void Clear();


	protected:
		sRule* FindRule(const int curState, const string &str);
		void RemoveNode(sNode *node);


	public:
		sNode* m_root;
		vector<sRule> m_rules;
	};


	//---------------------------------------------------------------------------------------
	// template function

#define FIND_ATTR(node, key, count)			\
		auto it = node->attrs.find(key);	\
		if (node->attrs.end() == it)		\
			return defaultValue;			\
		if (it->second.size() <= count)		\
			return defaultValue;


	template<class T>
	inline const T cSimpleData2::Get(cSimpleData2::sNode *node
		, const string &key, const T &defaultValue)
	{
		FIND_ATTR(node, key, 1);
		_variant_t v = (T)0;
		v = common::str2variant(v.vt, it->second[1]);
		return (T)v;
	}

	// template specialization (string)
	template<>
	inline const string cSimpleData2::Get(cSimpleData2::sNode *node
		, const string &key, const string &defaultValue)
	{
		FIND_ATTR(node, key, 1);
		return it->second[1];
	}

	// template specialization (Vector3)
	template<>
	inline const Vector3 cSimpleData2::Get(cSimpleData2::sNode* node
		, const string& key, const Vector3& defaultValue)
	{
		FIND_ATTR(node, key, 3);
		Vector3 v;
		v.x = (float)atof(it->second[1].c_str());
		v.y = (float)atof(it->second[2].c_str());
		v.z = (float)atof(it->second[3].c_str());
		return v;
	}
}
