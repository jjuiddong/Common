//
// 2015-12-05, jjuiddong
//
// 직선으로 나열된 3개의 LED를 인식해, 최대,최소 거리, 각도, 크기 등을
// 계산해, 최적의 파라메터를 찾아서 저장한다.
//
#pragma once


namespace cvproc
{

	class cLEDCalibrater
	{
	public:
		cLEDCalibrater();
		virtual ~cLEDCalibrater();

		bool Init(cSearchPoint *searchPoint, const int calibrateSeconds);
		bool Update(const cv::Mat &src);


		cSearchPoint *m_searchPoint;
		cRecognitionConfig m_tmpRecogConfig;

		int m_calibrateSeconds;
		int m_startCalibrateTime;

		float m_minLineLength;
		float m_maxLineLength;
		float m_maxLineDot;
		float m_maxHorizontalDot;
	};

}
