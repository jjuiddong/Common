//
// 2016-05-28, jjuiddong
// templatematch 기능
// 디버깅을 위해 각종 정보를 화면에 출력한다.
//
#pragma once


namespace cvproc
{

	class cTemplateMatch
	{
	public:
		cTemplateMatch(const float threshold=0.7f, const bool gray=true, const bool debug=false);
		virtual ~cTemplateMatch();
		bool Match(const cv::Mat &img, const cv::Mat &templ, 
			const string &comment="", OUT cv::Mat *out=NULL);
		bool Match(const string &srcFileName, const string &templFileName, 
			const string &option="", const string &comment = "");

	
	public:
		cv::Point m_leftTop;
		double m_min;
		double m_max;
		cv::Mat m_matResult;
		float m_threshold;
		bool m_gray;
		bool m_debug; // true이면 매칭 결과를 화면에 출력한다.
	};

}
