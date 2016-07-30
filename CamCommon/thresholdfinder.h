//
// 2015-12-05
// 
// 영상을 이진화 할 때, 사용하는 threshold 값을 자동으로 찾아주는 역할을 한다.
// 
// 적외선 LED 3개를 인식할 수 있는 threshold 값 중에, 가장 큰 것을 찾는다.
//
#pragma once


namespace cvproc
{

	class cThresholdFinder
	{
	public:
		cThresholdFinder();
		virtual ~cThresholdFinder();

		void Init(cSearchPoint *searchPoint);
		bool Update(const cv::Mat &src);


		// threshold calibration
		enum THRESHOLD_CALIBRATION_STATE
		{
			CALIB_UP,
			CALIB_DOWN,
			CALIB_COMPLETE,
		};

		THRESHOLD_CALIBRATION_STATE m_state;
		cSearchPoint *m_searchPoint; // reference
		cv::Mat m_binaryImg;
		int m_thresholdValue;
		int m_startCalibrationTime; // 컬리브레이션을 시작할 때의 시간 저장
		float m_thresholdMax; // IR LED를 인식할 수 있는 최대 thresold 값의 평균
		float m_oldThresholdMax;
		float m_thresholdVariance; // threshold 변화 편차를 저장한다.
		int m_varianceCount;
		float m_lpf; // low pass filter constant
		bool m_detectPoint; // LED가 발견되면 true가 된다.
	};

}
