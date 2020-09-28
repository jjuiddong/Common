//
// 2020-07-17, jjuiddong
// visual programming json file read/write
// convert json format to *.vprog format
//
#pragma once


namespace vprog
{
	class cVProgJsonFile : public cVProgFile
	{
	public:
		cVProgJsonFile();
		virtual ~cVProgJsonFile();

		bool ReadJson(const StrPath &fileName);
		bool ReadJsonRaw(const string &rawStr);
		bool WriteJson(const StrPath &fileName);
		void Clear();
	};
}
