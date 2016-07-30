//
// 2016-01-19, jjuiddong
//
//   * ------------ *
//   |            /
//   |         /
//   |      /
//   |    /
//   |  /
//   |/
//   *
//
// 3개의 점으로 둘러싸인 QR Code를 인식한다.
// 기존 QR Code와는 다르다.
//
//
#pragma once


namespace cvproc
{

	class cQRCode
	{
	public:
		cQRCode();
		virtual ~cQRCode();

		bool Init(const string &recogFileName);
		bool Detect(const cv::Mat &src);
		bool UpdateConfig(const string &recogFileName);


	protected:
		bool CheckTriangle(const vector<cv::Point> &points, vector<cv::Point> &out);


	public:
		bool m_dbgShow;
		cDetectPoint m_detectPoint;
		cRecognitionConfig m_recogCfg;
	};

}
