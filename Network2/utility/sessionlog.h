//
// 2019-03-10, jjuiddong
// Session Log
//	- *.ses file read
//	- format
//		- sessionname netid
//
#pragma once


namespace network2
{

	class cSessionLog
	{
	public:
		cSessionLog();
		virtual ~cSessionLog();

		bool Read(const char *fileName);
		const StrId& FindSession(const netid id);
		void Clear();


	public:
		VectorMap<netid, StrId> m_sessions;
	};

}
