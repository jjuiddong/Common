
#include "stdafx.h"
#include "templatematch.h"


using namespace cv;
using namespace cvproc;

void CallbackTemplateMatch(int event, int x, int y, int flags, void* userdata);


cTemplateMatch::cTemplateMatch(const float threshold, const bool gray, const bool debug) //threshold=0.7f, gray=true, debug=false
	: m_threshold(threshold)
	, m_gray(gray)
	, m_debug(debug)
{
}
cTemplateMatch::~cTemplateMatch()
{
}


// 이미지 매치
// out = 템플릿 매칭 결과 정보를 저장해 리턴한다.
bool cTemplateMatch::Match(const cv::Mat &img, const cv::Mat &templ, 
	const string &comment, cv::Mat *out) //comment="", out=NULL
{
	RETV(!img.data || !templ.data, false);
	RETV(img.channels() != templ.channels(), false);

	m_matResult = Mat(cvSize(img.cols - templ.cols + 1, img.rows - templ.rows + 1), img.flags);

	cv::matchTemplate(img, templ, m_matResult, CV_TM_CCOEFF_NORMED);
	cv::minMaxLoc(m_matResult, &m_min, &m_max, NULL, &m_leftTop);

	if (m_debug || out)
	{
		Mat src = img.clone();
		if (src.channels() == 1)
			cvtColor(src, src, CV_GRAY2BGR);
		stringstream ss;
		ss << comment << ", max=" << m_max;
		putText(src, ss.str(), Point(20, 30), 1, 2.f, Scalar(0, 0, 255), 2);
		cv::rectangle(src, m_leftTop, cvPoint(m_leftTop.x + templ.cols, m_leftTop.y + templ.rows), CV_RGB(255, 0, 0), 3);

		// 최대 화면 크기만큼, 이미지 사이즈를 조정한다.
		const int screenW = GetSystemMetrics(SM_CXSCREEN);
		const int screenH = GetSystemMetrics(SM_CYSCREEN);
		const int w = MIN(src.cols, screenW);
		const int h = MIN(src.rows, screenH);
		Mat dst(h, w, src.flags);
		resize(src, dst, Size(w, h));

		if (out)
		{
			*out = dst;
		}
		else
		{
			imshow("match result", dst);
			setMouseCallback("match result", CallbackTemplateMatch, NULL);
			waitKey(1);
		}
	}

	return m_max > m_threshold;
}


// 이미지 매치
bool cTemplateMatch::Match(const string &srcFileName, const string &templFileName, const string &option, const string &comment) 
// option="", comment=""
{
	Mat A = imread(srcFileName, (m_gray)? IMREAD_GRAYSCALE : IMREAD_COLOR);
	Mat B = imread(templFileName, (m_gray) ? IMREAD_GRAYSCALE : IMREAD_COLOR);
	RETV(!A.data, false);
	RETV(!B.data, false);

	if (!option.empty())
	{
		A = imread(srcFileName);
		cConvertImage cvt;
		cvt.Convert(A, A, option);
	}
	
	return Match(A, B, comment);
}


void CallbackTemplateMatch(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		destroyWindow("match result");
	}
}
