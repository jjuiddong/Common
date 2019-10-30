//
// 2019-10-30, jjuiddong
// Visual Programming File Read/Write
//	- converting to intermediate code
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
//	- output pin
//		- type string
//		- id num
//		- name string
//		- value string
//		- links num num num...
// symbol
//	- id num
//	- value string
//
//
#pragma once


namespace common
{
	namespace script
	{

		class cVProgFile
		{
		public:
			cVProgFile();
			virtual ~cVProgFile();

			bool Read(const StrPath &fileName);
			bool Write(const StrPath &fileName);


		public:
		};

	}
}
