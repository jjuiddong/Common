//
// 2016-05-28, jjuiddong
// Feature Matching
//
//	sample code
// https://fossies.org/dox/opencv-3.1.0/tutorial_feature_homography.html
//
#pragma once


namespace cvproc
{

	class cFeatureMatch
	{
	public:
		cFeatureMatch(const bool gray = true, const bool debug = false);
		virtual ~cFeatureMatch();

		bool Match(const string &srcFileName, const string &templFileName, 
			const string &option="", const string &comment = "");

		bool Match(const cv::Mat &img, const cv::Mat &templ, 
			const string &comment = "", OUT cv::Mat *out = NULL);

		bool SIFTMatch(const cv::Mat &img, const cv::Mat &templ, 
			const string &comment = "");

		bool Match(const cv::Mat &img, const vector<cv::KeyPoint> &imgKeyPoints,  const cv::Mat &imgDescriptors,
			const cv::Mat &templ, const vector<cv::KeyPoint> &templKeyPoints, const cv::Mat &templDescriptors,
			const string &comment = "", OUT cv::Mat *out = NULL);


	public:
		double m_min;
		double m_max;
		bool m_isLog;
		bool m_gray;
		bool m_debug; // true이면 매칭 결과를 화면에 출력한다.
		cv::Mat m_matResult;
		cRectContour m_rect;
		bool m_IsDetection;
	};

}