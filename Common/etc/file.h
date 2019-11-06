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
	};

}
