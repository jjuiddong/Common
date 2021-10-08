//
// 2019-11-19, jjuiddong
// simple data read/write class
//
// sample data file
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
//
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

#define FIND_ATTR(node, key)				\
		auto it = node->attrs.find(key);	\
		if (node->attrs.end() == it)		\
			return defaultValue;			\
		if (it->second.size() <= 1)			\
			return defaultValue;


	template<class T>
	inline const T cSimpleData2::Get(cSimpleData2::sNode *node
		, const string &key, const T &defaultValue)
	{
		FIND_ATTR(node, key);
		_variant_t v = (T)0;
		v = common::str2variant(v.vt, it->second[1]);
		return (T)v;
	}

	// template specialization (string)
	template<>
	inline const string cSimpleData2::Get(cSimpleData2::sNode *node
		, const string &key, const string &defaultValue)
	{
		FIND_ATTR(node, key);
		return it->second[1];
	}

}
