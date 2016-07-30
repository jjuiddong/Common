//
// 2016-01-22, jjuiddong
//
// 찌그러진 영역의 Point를 찾아 펼쳐진 상태에서의 좌표를 계산한다.
//
// 찌그러진 영역의 좌표 Point 4개와 Scale 값을 입력으로 한다.
//
#pragma once


class cSkewDetect
{
public:
	cSkewDetect(const int rows=480, const int cols=640);
	cSkewDetect(const cRecognitionConfig &recogConfig, const int rows = 480, const int cols = 640);
	virtual ~cSkewDetect();

	bool Init(const cRectContour &contour, const float scale, const int width=640, const int height=480);
	bool Read(const string &contourFileName, const bool createContourSize=false);
	bool Write(const string &contourFileName);
	bool Detect(const cv::Point pos, OUT cv::Point2f &out);
	cv::Mat& Transform(const cv::Mat &src);
	bool DetectCalc(const cv::Point pos, OUT cv::Point2f &out);


public:
	bool m_isInit;
	float m_scale;
	int m_rows;
	int m_cols;
	cSquareContour m_contour;
	cv::Mat m_transmtx;
	cv::Mat m_bkgnd;
	cv::Mat m_skewBkgnd;
	cDetectPoint m_detectPoint;
	cv::Point2f m_pos;		// 가장 최근 정보를 저장한다.
};

