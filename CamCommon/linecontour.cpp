
#include "stdafx.h"
#include "linecontour.h"


using namespace cv;


cLineContour::cLineContour() :
m_contours(4)
{

}

cLineContour::cLineContour(const cLineContour &rhs)
{
	operator=(rhs);
}

cLineContour::cLineContour(const vector<cv::Point> &contours) :
m_contours(4)
{
	Init(contours);
}

cLineContour::cLineContour(const vector<cv::KeyPoint> &contours) :
m_contours(4)
{
	Init(contours);
}


cLineContour::cLineContour(const cv::Point &center, const float size) :
m_contours(4)
{
	Init(center, size);
}

cLineContour::~cLineContour()
{

}


// 초기화.
bool cLineContour::Init(const vector<cv::Point> &contours)
{
	OrderedLineContours(contours, m_contours);
	return true;
}


// 초기화.
bool cLineContour::Init(const vector<cv::KeyPoint> &keypoints)
{
	if (keypoints.size() < 3)
		return false;

	vector<Point> keys(keypoints.size());
	for (u_int i = 0; i < keypoints.size(); ++i)
		keys[i] = Point(keypoints[i].pt);

	OrderedLineContours(keys, m_contours);

	return true;
}


// 중점 center를 중심으로 size만큼 키워진 선을 만든다.
//
// 0 -- 1 ---- 2 ---- + ----- 3 ----- 4
bool cLineContour::Init(const cv::Point &center, const float size)
{
	m_contours[0] = center + Point(-(int)(size / 2.f), 0);
	m_contours[1] = center + Point(-(int)(size / 4.f), 0);
	m_contours[2] = center + Point((int)(size / 4.f), 0);
	m_contours[3] = center + Point((int)(size / 2.f), 0);
	return true;
}


void cLineContour::Normalize()
{
	vector<Point> keys(m_contours.size());
	for (u_int i = 0; i < m_contours.size(); ++i)
		keys[i] = m_contours[i];
	OrderedLineContours(keys, m_contours);
}


// 박스 출력.
void cLineContour::Draw(cv::Mat &dst, const cv::Scalar &color, const int thickness, const bool isLoop)
// color = cv::Scalar(0, 0, 0), thickness = 1
{
	DrawLines(dst, m_contours, color, thickness, isLoop);
}


// 사각형의 중점을 리턴한다.
Point cLineContour::Center() const
{
	int cnt = 0;
	Point center;
	for each (auto &pt in m_contours)
	{
		if (pt != Point(0, 0))
		{
			center += pt;
			cnt++;
		}
	}

	if (cnt <= 0)
		return Point(0, 0);

	center = Point(center.x / cnt, center.y / cnt);
	return center;
}


// 선의 방향 벡터를 리턴한다.
Point2f cLineContour::Direction() const
{
	if (m_contours.empty())
		return Point2f(1, 0); // 기본값 리턴

	Point2f v = m_contours[1] - m_contours[0];
	return PointNormalize(v);
}


bool cLineContour::IsEmpty() const
{
	return ((m_contours[0] == Point(0, 0)) && (m_contours[1] == Point(0, 0)) &&
		(m_contours[2] == Point(0, 0)) && (m_contours[3] == Point(0, 0)));
}


cLineContour& cLineContour::operator=(const cLineContour &rhs)
{
	if (this != &rhs)
	{
		m_contours = rhs.m_contours;
	}
	return *this;
}


// 사각형의 중점을 중심으로 스케일한다.
// void cLineContour::ScaleCenter(const float scale)
// {
// 	const Point center = Center();
// 
// 	for (u_int i = 0; i < m_contours.size(); ++i)
// 	{
// 		m_contours[i] = center + ((m_contours[i] - center) * scale);
// 	}
// }


// 연속된 3개의 포인트를 찾아서 저장한다. 각 포인트의 위치의 최대 거리는 lineDistance 이다.
bool cLineContour::DetectLine(const vector<Point> &contours, 
	const double minLineDistance, const double maxLineDistance, 
	const float maxLineAngle, const float maxLineDot,
	int &idx1, int &idx2, int &idx3)
{
	if (m_contours.size() != 3)
		m_contours.resize(3);

	for (u_int i = 0; i < contours.size(); ++i)
	{
		for (u_int k = 0; k < contours.size(); ++k)
		{
			if (i==k)
				continue;

			const Point v1 = contours[k] - contours[i];
			const double d1 = cv::norm(v1);
			if ((d1 < minLineDistance) || (d1 > maxLineDistance))
				continue;

			const Point2f n1 = PointNormalize(v1);
			const float dot1 = n1.dot(Point2f(1, 0));
			if (abs(dot1) < maxLineAngle)
				continue;

			for (u_int m = 0; m < contours.size(); ++m)
			{
				if ((i == m) || (k == m))
					continue;

				const Point v2 = contours[m] - contours[k];
				const double d2 = cv::norm(v2);
				if ((d2 < minLineDistance) || (d2 > maxLineDistance))
				{
					//printf("min max Line Distance d2= %f\n", d2);
					continue;
				}
				
				const Point2f n2 = PointNormalize(v2);
				const float dt = n1.dot(n2);

				// 같은 방향으로 바라보면, 1
				// 1에서 얼마나 벗어났는지 판단.
				if (abs(dt - 1.f) > maxLineDot)
				{
					//printf("maxLineDot dt = %f\n", dt);
					continue;
				}

				idx1 = i;
				idx2 = k;
				idx3 = m;

				// 연속된 3개의 포인트를 찾으면, 함수를 종료한다.
				m_contours[0] = contours[i];
				m_contours[1] = contours[k];
				m_contours[2] = contours[m];
				return true;
			}
		}
	}

	return false;
}
