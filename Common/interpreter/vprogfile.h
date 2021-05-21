//
// 2019-10-30, jjuiddong
// Visual Programming File Read/Write
//	- convert to intermediate code
//	- common::script::cIntermediateCode
//
// read/write node, pin, link information
// ASCII format
//
// node
//	- name string 
//	- rect num num num num
//	- input
//		- type string
//		- id num
//		- name string
//		- value string
//		- links num num num...
//	- output
//		- type string
//		- id num
//		- name string
//		- value string
//		- links num num num...
// symbol
//	- id num
//	- value string
//
// 2020-11-17
//	- variable pin
//		- sPin::varName
//
// 2021-05-20
//	- symboltype refactoring
//	- add array<>, map<> type
//
//
#pragma once

#include "vprog.h"


namespace vprog
{

	class cVProgFile
	{
	public:
		struct sPin {
			int id;
			string name;
			string varName; // variable pin? variable symbol name
			int value; // enum value
			ePinType::Enum type;
			string typeStr; // type name
			vector<common::script::eSymbolType::Enum> typeValues;
			ePinKind::Enum kind;
			vector<int> links; // pin id array
		};

		struct sNode {
			int id;
			eNodeType::Enum type;
			string name;
			string desc; // description
			string labelName; // to set unique node name (especially, event label name)
			vector<sPin> inputs;
			vector<sPin> outputs;
		};

		struct sLink {
			int from; // from pin id
			int to; // to pin id
		};

		cVProgFile();
		virtual ~cVProgFile();

		bool Read(const StrPath &fileName);
		bool Write(const StrPath &fileName);
		bool GenerateIntermediateCode(OUT common::script::cIntermediateCode &out);
		void Clear();


	protected:
		bool Event_GenCode(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool Node_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Function_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Control_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Branch_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Switch_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool While_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool ForLoop_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Sequence_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool Operator_GenCode(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool Variable_GenCode(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool Pin_GenCode(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);
		bool Pin2_GenCode(const ePinKind::Enum kind
			, const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);

		bool TemporalPin_GenCode(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);

		bool DebugInfo_GenCode(const sPin &from, const sPin &to
			, OUT common::script::cIntermediateCode &out);
		bool DebugInfo_GenCode(const sNode &from, const sNode &to
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool NodeEnter_GenCode(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool NodeEscape_GenCode(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool NodeInput_GenCode(const sNode &node, const uint reg
			, const bool isUpdateInputPin, OUT common::script::cIntermediateCode &out);

		bool AddNode(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddVariable(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddSymbol(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddDefine(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddPin(const int parseState, sNode &node, const sPin &pin);
		bool AddVariable2(const string &scopeName, const string &name
			, const string &typeStr
			, common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);

		std::pair<sNode*,sPin*> FindContainPin(const int pinId);
		string MakeScopeName(const sNode &node, const int uniqueId = -1);
		uint GetInputFlowCount(const sNode &node);
		const sPin* GetInputPin(const sNode &node, const vector<string> &names);
		bool IsIgnoreInputPin(const ePinType::Enum type);

		bool Write_Node(std::ostream &ofs, sNode &node);
		bool Write_Define(std::ostream &ofs, sNode &node);


	public:
		StrPath m_fileName;
		vector<sNode> m_nodes;
		vector<sLink> m_links;
		common::script::cSymbolTable m_variables;
		set<int> m_visit; // visit node, use generate intermediate code
		int m_jmpLabelSeedId; // to generate unique jump label id
	};

}

