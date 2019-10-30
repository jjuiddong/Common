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


namespace framework {
	namespace vprog {

		class cNodeFile
		{
		public:
			cNodeFile();
			virtual ~cNodeFile();

			bool Read(const StrPath &fileName);
			bool Write(const StrPath &fileName);


		protected:
			sPin* FindPin(const ed::PinId id);
			bool AddPin(const int parseState, cNode &node, const sPin &pin);


		public:
			vector<cNode> m_nodes;
			vector<sLink> m_links;
			cSymbolTable m_symbTable;
		};
	}
}
