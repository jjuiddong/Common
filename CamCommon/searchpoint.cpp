
#include "stdafx.h"
#include "searchpoint.h"


using namespace cv;
using namespace cvproc;


cSearchPoint::cSearchPoint(const int id)
	: m_oldRawPos(0,0)
	, m_maxDiffCount(0)
	, m_skewWidth(0)
	, m_skewHeight(0)
	, m_detectPoint(common::format("BinaryWindow%d", id))
	, m_show(false)
{
	m_show = g_config.m_conf.showDetectPoint;
	m_windowName = common::format("SkewWindow%d", id);

	if (m_show)
	{
		namedWindow(m_windowName, 0);
		cvResizeWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.width, g_config.m_detectPointBinaryScreenRect.height);
		cvMoveWindow(m_windowName.c_str(), g_config.m_detectPointBinaryScreenRect.x, g_config.m_detectPointBinaryScreenRect.y);
	}
}

cSearchPoint::~cSearchPoint()
{
}


bool cSearchPoint::Init(const string &recognitionConfigFile, const string &contourConfigFile, const string &roiFile)
{
	if (!m_recogConfig.Read(recognitionConfigFile))
		return false;

	if (!m_skewDetect.Read(contourConfigFile, true))
		return false;

	if (!m_roi.Read(roiFile))
		return false;

	m_detectPoint.UpdateParam(m_recogConfig);

	Size size(m_skewDetect.m_contour.Width(), m_skewDetect.m_contour.Height());
	m_skewWidth = m_skewDetect.m_contour.Width();
	m_skewHeight = m_skewDetect.m_contour.Height();

	m_skewDetect.m_skewBkgnd.create(size, CV_8UC(3));	
	return true;
}


bool cSearchPoint::InitSkewRect(const cRectContour &contour, const float scale)
{
	m_skewDetect.Init(contour, scale, contour.Width(), contour.Height());
	m_skewWidth = contour.Width();
	m_skewHeight = contour.Height();

	return true;
}


bool cSearchPoint::ReadContourFile(const string &contourConfigFile)
{
	if (!m_skewDetect.Read(contourConfigFile, true))
		return false;

	Size size(m_skewDetect.m_contour.Width(), m_skewDetect.m_contour.Height());
	m_skewWidth = m_skewDetect.m_contour.Width();
	m_skewHeight = m_skewDetect.m_contour.Height();

	m_skewDetect.m_skewBkgnd.create(size, CV_8UC(3));
	return true;
}


bool cSearchPoint::RecognitionSearch(const Mat &src, OUT Point &rawPos, const bool isSkewTransform, const bool isRoi)
// isSkewTransform=true, isRoi=false
{
	if (src.empty())
		return false;

	// Roi
	const Mat *psrc = (isRoi) ? &m_roi.Update(src) : &src;

	const Mat *srcImage = NULL;
	if (isSkewTransform)
	{
		m_skewDetect.Transform(*psrc);
		srcImage = &m_skewDetect.m_skewBkgnd;
	}
	else
	{
		srcImage = psrc;
	}

	if (m_show && !srcImage->empty())
		imshow(m_windowName, *srcImage);

	// IR LED 인식
	Point tmpPt;
	if (m_detectPoint.DetectPoint2(*srcImage, tmpPt, false))
	{
		int loopCnt = 0;
		cLineContour bestLine;
		int minimumLenght = 0;
		vector<Point> detectPoints = m_detectPoint.GetDetectPoints();
		if (detectPoints.size() <= 2)
		{
			goto notdetect;
		}

		// 그전 포인터와 가장 근접한 포인트를 찾는다.
		while (1 && (loopCnt < m_recogConfig.m_attr.detectPointLoopCount))
		{
			++loopCnt;

			// 세 포인트가 직선인지 검사한다.
			int idx1, idx2, idx3;
			cLineContour lineContour;
			if (lineContour.DetectLine(detectPoints,
				m_recogConfig.m_attr.detectPointMinLineDist, m_recogConfig.m_attr.detectPointMaxLineDist,
				m_recogConfig.m_attr.detectLineMinAngle, m_recogConfig.m_attr.detectLineMaxDot,
				idx1, idx2, idx3))
			{
				const Point diff = lineContour.Center() - m_oldRawPos;
				const int distance = diff.x*diff.x + diff.y*diff.y;

				if (bestLine.IsEmpty())
				{
					bestLine = lineContour;
					minimumLenght = distance;
				}
				else
				{
					if (minimumLenght > distance)
					{
						bestLine = lineContour;
						minimumLenght = distance;
					}
				}

				// 인식된 포인트는 제거된다.
				// 인덱스를 정렬해서, 큰것부터 제거해야 문제가 없다.
				vector<int> indices(3);
				indices[0] = idx1;
				indices[1] = idx2;
				indices[2] = idx3;
				std::sort(indices.begin(), indices.end(),
					[](int const& a, int const& b) { return a > b; });

				rotatepopvector(detectPoints, indices[0]);
				rotatepopvector(detectPoints, indices[1]);
				rotatepopvector(detectPoints, indices[2]);
			}
			else
			{
				break;
			}
		}

		if (bestLine.IsEmpty())
		{
			goto notfind_line;
		}
		else
		{
			const Point pt = bestLine.Center();
			const Point diff = pt - m_oldRawPos;
			const float distance = (float)sqrt(diff.x*diff.x + diff.y*diff.y);

			bool IsUpdatePos = true;
			if (distance > m_recogConfig.m_attr.detectLineMinDiffDistance)
			{	// 전 보다 너무 크게 벗어나면, 검증해본다.
				// bestLine 에서 가장 가까운 포인트를 찾아 거리값을 구한다.
				// 오리지널 포인트라면, 가장 가까운 포인트와의 거리는 멀다.
				// 잘못된 포인트라면, 근처에 가까운 포인트가 있을 확률이 높다.

				++m_maxDiffCount;

				// 새로 찾은 라인과 그전 라인과의 각도가 크면 틀릴 확률이 높다.
				if (!m_oldBestLine.IsEmpty())
				{
					const float dot2 = m_oldBestLine.Direction().dot(bestLine.Direction());
					if ((abs(dot2) < m_recogConfig.m_attr.detectLineMinOldAngle) && (m_maxDiffCount < 20))
						IsUpdatePos = false;
				}
			}

			if (IsUpdatePos) // Line Detect, 정보 업데이트
			{
				rawPos = pt;
				m_maxDiffCount = 0;
				m_oldBestLine = bestLine;
			}

			return true;
		}
	}

	return false;

notdetect:
	return false;

notfind_line:
	return false;
}


// SkewTransform된 화면에서의 pos 값을 uv 좌표로 변환해 리턴한다.
//
//  +--------------+
//  | (0,1)        |(1,1)
//  |              |
//  |       +      |
//  |              |
//  |(0,0)         |(1,0)
//  +--------------+
//
void cSearchPoint::ConvertUV(const Point &pos, OUT float &outX, OUT float &outY)
{
	if ((m_skewWidth == 0) || (m_skewHeight == 0))
		return;

	float x = pos.x / (float)m_skewWidth;
	float y = 1.f - (pos.y / (float)m_skewHeight);
	x = (x - 0.5f) * m_skewDetect.m_scale + 0.5f;
	y = (y - 0.5f) * m_skewDetect.m_scale + 0.5f;

	x = clamp(0, 1, x);
	y = clamp(0, 1, y);

	outX = x;
	outY = y;
}
