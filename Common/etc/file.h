//
// 2019-09-03, jjuiddong
// file operation class
//
#pragma once


namespace common
{

	class cFile
	{
	public:
		cFile();
		virtual ~cFile();

		bool ReadFile2String(const StrPath &fileName
			, OUT string &out);

		static int ReadAll(const StrPath& fileName, const bool isBinary
			, const uint size, OUT char* dest);

		static std::pair<uint, char*> ReadAll2(const StrPath& fileName
			, const bool isBinary);
	};

}
