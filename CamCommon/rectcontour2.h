//
// 사각 박스를 표현하는 클래스.
// cRectContour 클래스를 vector를 쓰지 않고, 배열을 쓰게 함.
// OrderedContours() 후, 인덱스 변화를 저장한다.
//
#pragma once


class cRectContour2
{
public:
	cRectContour2();
	cRectContour2(const vector<cv::Point> &contours);
	cRectContour2(const vector<cv::KeyPoint> &contours);
	cRectContour2(const cv::Point &center, const float size);
	virtual ~cRectContour2();

	bool Init(const cv::Point contours[4]);
	bool Init(const vector<cv::Point> &contours);
	bool Init(const vector<cv::Point2f> &contours);
	bool Init(const vector<cv::KeyPoint> &keypoints);
	bool Init(const cv::Point &center, const float size);
	void Normalize();
	void Draw(cv::Mat &dst, const cv::Scalar &color = cv::Scalar(0, 0, 0), const int thickness = 1);

	int Width() const;
	int Height() const;
	cv::Point At(const int index) const;
	cv::Point Center() const;
	void ScaleCenter(const float scale);
	void Scale(const float vscale, const float hscale);

	cRectContour2& operator = (const cRectContour2 &rhs);


public:
	cv::Point m_contours[4];
	int m_chIndices[4];
};
