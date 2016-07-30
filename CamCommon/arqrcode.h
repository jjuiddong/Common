//
// 2016-01-21, jjuiddong
//
// 증강현실에서 사용되는 QRCode 인식 클래스
//
//  * ------------ *
//  |              |
//  |              |
//  |              |
//  |           *  |
//  * ------------ *
//
#pragma once

#include "detectrect.h"


namespace cvproc
{

	class cArQRCode
	{
	public:
		cArQRCode();
		virtual ~cArQRCode();

		bool Init(const string &recogFileName);
		bool Detect(const cv::Mat &src);
		bool UpdateConfig(const string &recogFileName);


	public:
		bool m_dbgShow;
		cDetectRect m_detectEdge;
		cRecognitionEdgeConfig m_recogCfg;
		cSkewDetect m_skewDetect;
	};

}
