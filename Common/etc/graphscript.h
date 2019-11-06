//
// 2019-09-03, jjuiddong
// graph script
//	xy plot 그래프를 그리기 위한 기본적인 정보를 스크립트로 읽는 기능을 한다.
//
// graph information
//	- graph name
//	- parsing format
//
// Graph Script Sample
//	plot = %f;
//	name = Yaw
//	plot = %*f; %f;
//	name = Pitch
//	plot = %*f; %*f; %f;
//	name = Roll
//
#pragma once


namespace common
{

	class cGraphScript
	{
	public:
		struct sGraphFormat {
			int type; // 0:string, 1:binary
			StrId name;
			Str64 fmt; // type:0, string parsing, sscanf arg
			uint idx; // type:1, array index
		};

		cGraphScript();
		virtual ~cGraphScript();

		bool ReadFile(const StrPath &fileName);
		bool ReadString(const string &strScript);
		void Clear();


	public:
		vector<sGraphFormat> m_graphs;
	};

}
