//
// 2020-07-17, jjuiddong
// visual programming json file read/write
// convert json format to *.vpl format
//
//
// 2021-05-09
//	- change name cVProgJsonFile -> cVplJsonFile
//
#pragma once


namespace vpl
{
	class cVplJsonFile : public cVplFile
	{
	public:
		cVplJsonFile();
		virtual ~cVplJsonFile();

		bool ReadJson(const StrPath &fileName);
		bool ReadJsonRaw(const string &rawStr);
		bool WriteJson(const StrPath &fileName);
		void Clear();
	};
}
