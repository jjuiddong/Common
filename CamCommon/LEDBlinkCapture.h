//
// 2015-12-07, jjuiddong
//
// LED를 깜빡거리게해서, 불이 꺼졌을 때, 주변광들의 위치를 추적해,
// 불이 켜졌을 때, 주변광들의 제거하는 기능을 한다.
//
#pragma once


namespace cvproc
{
	
	class cLEDBlinkCapture
	{
	public:
		cLEDBlinkCapture();
		virtual ~cLEDBlinkCapture();

		bool Init(const int portNum, const int baudRate, const int delayMilliseconds);
		void Update(const cv::Mat &src);
		void LEDOn();
		void LEDOff();


	protected:
		void DetectIgnorePoint(const cv::Mat &src);
		

	public:
		cSerial m_serial;
		int m_delayMilliseconds;
		int m_oldT;
		bool m_chCapture;
		bool m_chToggle;
		bool m_toggle;

		cv::Mat m_ledOn;
		cv::Mat m_ledOff;
	};

}
