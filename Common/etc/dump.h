//
// 2023-07-09, jjuiddong
// minidump class
//	- create dump file when crash program
//
#pragma once


namespace common 
{
	namespace dbg 
	{
		class cDump
		{
		public:
			cDump();
			virtual ~cDump();
			bool Init(const string& name);
			void Clear();


		protected:
			static LONG WINAPI ExceptionCallback(
				__in struct _EXCEPTION_POINTERS* exceptioninfo);


		public:
			static string s_name; // dump name
		};
	}
}
