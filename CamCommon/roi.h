#pragma once


namespace cvproc
{

	class cRoi
	{
	public:
		cRoi();
		cRoi(const cv::Rect &rect);

		cv::Mat& Update(const cv::Mat &src);
		bool Read(const string &fileName);
		bool Write(const string &fileName);


		cv::Rect m_rect;
		cv::Mat m_final;
		cv::Mat m_matRoi;
	};

}
