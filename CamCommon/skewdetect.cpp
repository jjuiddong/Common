
#include "stdafx.h"
#include "skewdetect.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace cv;


cSkewDetect::cSkewDetect(const int rows, const int cols)
	: m_rows(rows)
	, m_cols(cols)
{
}

cSkewDetect::cSkewDetect(const cRecognitionConfig &recogConfig, const int rows, const int cols)
	: m_rows(rows)
	, m_cols(cols)
	, m_bkgnd(Size(cols, rows), CV_8UC(3))
	, m_skewBkgnd(Size(cols, rows), CV_8UC(3))
	, m_isInit(false)
	, m_detectPoint(recogConfig, "SkewDetect Binary")
	, m_pos(0, 0)
	, m_scale(2.3f)
{
	m_detectPoint.m_params.filterByConvexity = false;
	m_detectPoint.m_params.filterByCircularity = false;
	m_detectPoint.m_params.minArea = 1;
	m_detectPoint.m_params.maxArea = 300;
	m_detectPoint.m_params.minConvexity = 0.03f;
	m_detectPoint.m_params.minInertiaRatio = 0.001f;
}

cSkewDetect::~cSkewDetect()
{
}


// 초기화
// contour 값은 이미 스케일링 되었다고 가정한다.
bool cSkewDetect::Init(const cRectContour &contour, const float scale, const int width, const int height)
{
	m_isInit = true;
	m_contour.Init(contour.m_contours);
	m_contour.InitSquare();
	m_scale = scale;

	if ((m_skewBkgnd.cols != width) || (m_skewBkgnd.rows != height))
	{
		m_rows = height;
		m_cols = width;
		m_skewBkgnd = Mat(Size(width, height), CV_8UC(3));
		m_bkgnd.setTo(Scalar(255, 255, 255));
		m_skewBkgnd.setTo(Scalar(255, 255, 255));
	}

	m_transmtx = SkewTransform(m_contour.m_contours, width, height, 1);
	return true;
}


// src 위치상에 pos에서 포착된 점을, m_contour영역을 통해 펼친 후, 새 좌표를
// 계산해서 out에 저장해서 리턴한다.
// 점을 찾지 못했다면, 그 전 좌표를 리턴한다.
// out : x,y -> 0~1 값을 리턴한다.
//		 왼쪽 아래가 0,0 이다.
bool cSkewDetect::Detect(const Point pos, OUT Point2f &out)
{
	if (!m_isInit)
		return false;
	if (m_skewBkgnd.empty())
		return false;
	if (m_transmtx.empty())
		return false;

	m_bkgnd.setTo(Scalar(0, 0, 0));
	circle(m_bkgnd, pos, 2, Scalar(255, 255, 255), CV_FILLED);
	cv::warpPerspective(m_bkgnd, m_skewBkgnd, m_transmtx, m_skewBkgnd.size());

	//----------------------------------------------------------------------------------------------
	// LED 인식
	Point tmpPt;
	if (m_detectPoint.DetectPoint(m_skewBkgnd, tmpPt, false))
	{
		Point2f tPos = tmpPt;
		tPos.y = m_skewBkgnd.rows - tPos.y;
		tPos.x /= m_skewBkgnd.cols;
		tPos.y /= m_skewBkgnd.rows;

		// scaling
		tPos = ((tPos - Point2f(0.5f, 0.5f)) * m_scale) + Point2f(0.5f, 0.5f);

		tPos.x = clamp(0, 1, tPos.x);
		tPos.y = clamp(0, 1, tPos.y);

		out = tPos;
		m_pos = tPos; // 가장 최근 정보 저장
	}
	else
	{
		out = m_pos;
	}

	return true;
}


// src이미지를 skew transform을 적용해 리턴한다.
Mat& cSkewDetect::Transform(const Mat &src)
{
	if (!m_isInit)
		return m_skewBkgnd;
	if (m_skewBkgnd.empty())
		return m_skewBkgnd;
	if (m_transmtx.empty())
		return m_skewBkgnd;
	
	cv::warpPerspective(src, m_skewBkgnd, m_transmtx, m_skewBkgnd.size());
	return m_skewBkgnd;
}


// 계산을 통해 위치를 추정한다.
bool cSkewDetect::DetectCalc(const Point pos, OUT Point2f &out)
{
	Point2f tmpPt;
	if (m_contour.GetUV(pos, tmpPt))
	{
		Point2f tPos = tmpPt;

		// scaling
		tPos = ((tPos - Point2f(0.5f, 0.5f)) * m_scale) + Point2f(0.5f, 0.5f);

		tPos.x = clamp(0, 1, tPos.x);
		tPos.y = clamp(0, 1, tPos.y);

		out = tPos;
		m_pos = tPos;
		return true;
	}
	else
	{
		out = m_pos;
		return false;
	}
}


// contour 파일을 읽는다.
// 파일에서 skewdetect 정보를 읽어드린다.
bool cSkewDetect::Read(const string &contourFileName, const bool createContourSize)
{
	cout << "read SkewDetect file... ";

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(contourFileName, props);

		if (props.get<string>("format", "") != "camera contour")
		{
			cout << "Fail " << endl;
			return false;
		}

		float scale;
		cRectContour contour;
		vector<Point2f> pos(4);

		scale = props.get<float>("scale", 1);
		pos[0].x = props.get<float>("pos1x", 1);
		pos[0].y = props.get<float>("pos1y", 1);
		pos[1].x = props.get<float>("pos2x", 1);
		pos[1].y = props.get<float>("pos2y", 1);
		pos[2].x = props.get<float>("pos3x", 1);
		pos[2].y = props.get<float>("pos3y", 1);
		pos[3].x = props.get<float>("pos4x", 1);
		pos[3].y = props.get<float>("pos4y", 1);

		contour.Init(pos);

		int width = m_skewBkgnd.empty() ? m_cols : m_skewBkgnd.cols;
		int height = m_skewBkgnd.empty() ? m_rows : m_skewBkgnd.rows;
		if (createContourSize)
		{
			width = contour.Width();
			height = contour.Height();
		}

		Init(contour, scale, width, height);

		cout << "Ok" << endl;
	}
	catch (std::exception&e)
	{
		cout << "Fail " << e.what() << endl;
	}

	return true;
}


// 파일에 skewdetect 정보를 쓴다.
bool cSkewDetect::Write(const string &contourFileName)
{
	cout << "write SkewDetect file... ";

	if (contourFileName.empty())
		return false;

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;

		props.add<string>("format", "camera contour");

		props.add<float>("scale", m_scale);
		props.add<int>("pos1x", m_contour.m_contours[0].x);
		props.add<int>("pos1y", m_contour.m_contours[0].y);
		props.add<int>("pos2x", m_contour.m_contours[1].x);
		props.add<int>("pos2y", m_contour.m_contours[1].y);
		props.add<int>("pos3x", m_contour.m_contours[2].x);
		props.add<int>("pos3y", m_contour.m_contours[2].y);
		props.add<int>("pos4x", m_contour.m_contours[3].x);
		props.add<int>("pos4y", m_contour.m_contours[3].y);

		boost::property_tree::write_json(contourFileName, props);
	}
	catch (std::exception&e)
	{
		cout << "Fail " << e.what() << endl;
	}

	return true;
}
