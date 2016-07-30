
#include "stdafx.h"
#include "detectrect.h"


using namespace cv;
using namespace cvproc;

cDetectRect::cDetectRect() :
	m_show(false)
	, m_threshold1(100)
	, m_threshold2(200)
	, m_minArea(400)
	, m_minCos(-0.4f)
	, m_maxCos(0.4f)
{
}

cDetectRect::~cDetectRect()
{
}


bool cDetectRect::Init()
{
	return true;
}


void cDetectRect::UpdateParam(const cRecognitionEdgeConfig &recogConfig)
{
	m_threshold1 = recogConfig.m_attr.threshold1;
	m_threshold2 = recogConfig.m_attr.threshold2;
	m_minArea = recogConfig.m_attr.minArea;
	m_minCos = recogConfig.m_attr.minCos;
	m_maxCos = recogConfig.m_attr.maxCos;
}


// 사각형 인식
bool cDetectRect::Detect(const Mat &src)
{
	bool isDetect = false;

 	cvtColor(src, m_gray, CV_RGB2GRAY);	
 	threshold(m_gray, m_binMat, m_threshold1, 255, cv::THRESH_BINARY);
	Canny(m_binMat, m_edges, 0, m_threshold2, 5);
	findContours(m_edges, m_contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;

	for (u_int i = 0; i < m_contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(m_contours[i]), approx, cv::arcLength(cv::Mat(m_contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(m_contours[i])) < m_minArea || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 4)
		{
			// Number of vertices of polygonal curve
			const int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			const double mincos = cos.front();
			const double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if ((vtc == 4) && (mincos >= m_minCos) && (maxcos <= m_maxCos))
			{
				vector<Point> rectPoint(4);
				rectPoint[0] = approx[0];
				rectPoint[1] = approx[1];
				rectPoint[2] = approx[2];
				rectPoint[3] = approx[3];
				m_rect.Init(rectPoint);

				setLabel(m_edges, "RECT", m_contours[i]);
				m_rect.Draw(m_edges, Scalar(255, 0, 0), 2);

				isDetect = true;
			}
		}
	}

	if (m_show)
	{
		imshow("detectrect bin", m_binMat);
		imshow("detectrect edges", m_edges);
	}

	return isDetect;
}
