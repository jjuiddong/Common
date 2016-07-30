//
// 라인을 표현하는 클래스.
//
#pragma once


class cLineContour
{
public:
	cLineContour();
	cLineContour(const cLineContour &rhs);
	cLineContour(const vector<cv::Point> &contours);
	cLineContour(const vector<cv::KeyPoint> &contours);
	cLineContour(const cv::Point &center, const float size);
	virtual ~cLineContour();

	bool Init(const vector<cv::Point> &contours);
	bool Init(const vector<cv::KeyPoint> &keypoints);
	bool Init(const cv::Point &center, const float size);
	bool DetectLine(const vector<cv::Point> &contours, 
		const double minLineDistance, const double maxLineDistance, 
		const float maxLineAngle, const float maxLineDot,
		int &idx1, int &idx2, int &idx3);
	void Normalize();
	void Draw(cv::Mat &dst, const cv::Scalar &color = cv::Scalar(0, 0, 0), const int thickness = 1, 
		const bool isLoop=false);

	bool IsEmpty() const;
	cv::Point Center() const;
	cv::Point2f Direction() const;
//	void ScaleCenter(const float scale);
	cLineContour& operator=(const cLineContour &rhs);


//protected:
	vector<cv::Point> m_contours;
};
