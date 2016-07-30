
#include "stdafx.h"
#include "thresholdfinder.h"


using namespace cv;
using namespace cvproc;


cThresholdFinder::cThresholdFinder() :
	m_searchPoint(NULL)
	, m_state(CALIB_DOWN)
	, m_lpf(0.8f)
	, m_detectPoint(false)
{
}

cThresholdFinder::~cThresholdFinder()
{
}


void cThresholdFinder::Init(cSearchPoint *searchPoint)
{
	m_state = CALIB_DOWN;
	m_searchPoint = searchPoint;
	m_oldThresholdMax = 0;
	m_thresholdMax = 0;
	m_thresholdVariance = 0;
	m_varianceCount = 0;
	m_thresholdValue = 255;
	m_startCalibrationTime = timeGetTime();
}


// IR LED가 인식 될 때까지 threshold를 바꾼다.
// IR LED가 인식되면, 인식이 안될 때까지 조정한다. 이렇게 하면서
// min, max threshold를 구하고, 최적의 threshold를 구한다.
bool cThresholdFinder::Update(const cv::Mat &src)
{
	RETV(!m_searchPoint, false);

	switch (m_state)
	{
	case CALIB_UP:
	{
		++m_thresholdValue;
		if (m_thresholdValue > 255)
		{
			m_thresholdValue = 255;
			m_state = CALIB_DOWN;
		}
	}
	break;

	case CALIB_DOWN:
	{
		--m_thresholdValue;
		if (m_thresholdValue < 0)
		{
			m_thresholdValue = 0;
			m_state = CALIB_UP;
		}
	}
	break;
	}

	Mat grayscaleMat = src.clone();
	cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환
	if (m_binaryImg.empty())
		m_binaryImg.create(grayscaleMat.size(), grayscaleMat.type());
	threshold(grayscaleMat, m_binaryImg, m_thresholdValue, 255, cv::THRESH_BINARY_INV);

	Point pos;
	m_detectPoint = false;
	if (m_searchPoint->RecognitionSearch(src, pos, false))
	{
		m_detectPoint = true;

		switch (m_state)
		{
		case CALIB_UP:
		case CALIB_DOWN:
		{
			// threshold를 조금 낮춰서 맞춘다.
			const int threshold = m_thresholdValue - m_searchPoint->m_recogConfig.m_attr.thresholdFinderOffset;
			if (m_thresholdMax == 0)
				m_thresholdMax = (float)(threshold);
			else
				m_thresholdMax = m_lpf*m_thresholdMax + ((1 - m_lpf)*threshold);

			// 표준 편차를 구한다.
			const float var = sqrt((m_thresholdMax - (float)threshold) * (m_thresholdMax - (float)threshold));
			m_thresholdVariance = m_lpf*m_thresholdVariance + ((1 - m_lpf)*var);

			// threshold 값이 안정적이라면 컬리브레이션을 종료한다.
			if (m_thresholdVariance < 1.1f)
			{
				++m_varianceCount;

				if (m_varianceCount > 100)
				{
					// 표준편차가 1에 가깝다면 종료
					m_thresholdValue = (int)m_thresholdMax;
					m_state = CALIB_COMPLETE;
				}
			}
			else
			{
				m_varianceCount = 0;
			}

			if (m_state != CALIB_COMPLETE)
			{
				m_oldThresholdMax = m_thresholdMax;
				m_state = CALIB_DOWN;
				m_thresholdValue = 255;
			}
		}
		break;
		}
	}

	if (m_state == CALIB_COMPLETE)
		return true;

	return false;
}

