//
// 2019-10-26, jjuiddong
// Visual Programming Node File
//
// read/write node, pin, link information
// ASCII format
//
// node
//	- name
//	- rect
//	- input pin
//		- type
//		- id
//		- name
//		- value
//		- links
//			- pinid
//	- output pin
//		- type
//		- id
//		- name
//		- value
//		- links
//			- pinid
//
#pragma once


namespace vprog 
{
	namespace script = common::script;

	class cNodeFile
	{
	public:
		cNodeFile();
		virtual ~cNodeFile();

		bool Read(const StrPath &fileName);
		bool Write(const StrPath &fileName);
		void Clear();


	protected:
		bool Write_Node(std::ostream &ofs, cNode &node);
		bool Write_Define(std::ostream &ofs, cNode &node);
		sPin* FindPin(const ed::PinId id);
		std::pair<cNode*, sPin*> FindContainPin(const ed::PinId id);
		bool GetPinVar(const sPin &pin, const string &value
			, OUT _variant_t &out);


	public:
		vector<cNode> m_nodes;
		vector<sLink> m_links;
		common::script::cSymbolTable m_symbTable;
	};
}
