//
// 2016-03-17, jjuiddong
// 
// ARToolkit 엔진 제어
//
//
//

#pragma once


namespace ar
{

	class cAREngine : public common::cSingleton<cAREngine>
	{
	public:
		cAREngine();
		virtual ~cAREngine();

		bool Init(int argc, char **argv);
		

	public:
		ARHandle *m_arHandle;
		ARPattHandle *m_arPattHandle;
		AR3DHandle *m_ar3DHandle;
		ARGViewportHandle *m_vp;
		ARParamLT *m_CparamLT;
		int m_xsize;
		int m_ysize;
	};

}
