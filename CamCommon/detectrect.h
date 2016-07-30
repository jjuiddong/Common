//
// 2016-01-21, jjuiddong
//
// 사각형을 찾는다.
//
#pragma once


namespace cvproc
{

	class cDetectRect
	{
	public:
		cDetectRect();
		virtual ~cDetectRect();

		bool Init();
		bool Detect(const cv::Mat &src);
		void UpdateParam(const cRecognitionEdgeConfig &recogConfig);


	public:
		bool m_show;
		cRectContour m_rect;

		int m_threshold1;
		int m_threshold2;
		int m_minArea;
		double m_minCos;
		double m_maxCos;

		vector<vector<cv::Point> > m_contours;

		cv::Mat m_gray;
		cv::Mat m_edges;
		cv::Mat m_binMat;
	};

}
