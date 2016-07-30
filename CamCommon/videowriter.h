//
// 2015-12-29, jjuiddong
//
// 영상을 녹화하는 기능을 한다. 
// 프레임에 맞춰서 녹화 한다. 기본적으로 30프레임으로 저장한다.
//
#pragma once


namespace cvproc
{

	class cVideoWriter
	{
	public:
		cVideoWriter();
		virtual ~cVideoWriter();

		bool Init(const string &fileName, const int width=640, const int height=480);
		void Record(const cv::Mat &img);
		void Close();

		
		enum {SAVE_FPS=15};
		CvVideoWriter *m_videoWriter;
		int m_lastRecordTime;
	};

}
