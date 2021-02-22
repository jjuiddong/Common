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
			string typeStr; // type name
			string varName; // variable pin? variable symbol name
			int value; // enum value
			ePinType::Enum type;
			eSymbolType::Enum subType0; // array element type, map key type
			eSymbolType::Enum subType1; // map value type
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
		bool GenerateCode_Event(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Node(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Function(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Control(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Branch(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Switch(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_While(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_ForLoop(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Sequence(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Operator(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Variable(const sNode &node
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Pin(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_Pin2(const ePinKind::Enum kind
			, const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);

		bool GenerateCode_TemporalPin(const sNode &node, const sPin &pin, const uint reg
			, OUT common::script::cIntermediateCode &out);

		bool GenerateCode_DebugInfo(const sPin &from, const sPin &to
			, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_DebugInfo(const sNode &from, const sNode &to
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);
		bool GenerateCode_NodeEnter(const sNode &prevNode, const sNode &node
			, const sPin &fromPin, OUT common::script::cIntermediateCode &out);


		bool AddNode(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddVariable(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddSymbol(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddDefine(common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);
		bool AddPin(const int parseState, sNode &node, const sPin &pin);
		bool AddVariable2(const string &scopeName, const string &name
			, const ePinType::Enum type, const string &typeStr
			, common::cSimpleData2 &sdata, common::cSimpleData2::sNode *p);

		std::pair<sNode*,sPin*> FindContainPin(const int pinId);
		string MakeScopeName(const sNode &node);
		uint GetInputFlowCount(const sNode &node);
		bool Write_Node(std::ostream &ofs, sNode &node);
		bool Write_Define(std::ostream &ofs, sNode &node);


	public:
		StrPath m_fileName;
		vector<sNode> m_nodes;
		vector<sLink> m_links;
		common::script::cSymbolTable m_variables;
		set<int> m_visit; // visit node, use generate intermediate code
	};

}

